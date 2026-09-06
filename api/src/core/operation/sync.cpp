#include "core.hpp"

struct SyncArgs {
    Paths*        src_paths;
    Paths*        dest_paths;
    object::info* src;
    object::info* dest;
    object::info* common;
};

void insert_conflict(LvcConflict& src, LvcConflictArray& dest, char** error_message) {
    LvcConflict* tmp = (LvcConflict*)realloc(dest.conflict, (dest.length + 1) * sizeof(LvcConflict));
    if (!tmp) {
        error_message_creator("Memory allocation failed", error_message);
        return;
    }
    dest.conflict              = tmp;
    dest.conflict[dest.length] = {src.a, src.b, src.relative_path};
    dest.length++;
}

LvcConflict new_file(const SyncArgs& args, char** error_message) {
    std::string buffer = io::content((args.src_paths->object) / args.dest->id, 1, error_message);
    size_t      pos    = buffer.find('\n');
    io::file(
        args.src_paths->root / args.dest->path, std::ios::binary, buffer.data() + pos + 1, buffer.size() - pos - 1, 0,
        error_message);
    return {};
}

LvcConflict existing_file(const SyncArgs& args, char** error_message) { return {}; }

LvcConflict no_common(const SyncArgs& args, char** error_message) { return {}; }

LvcConflict sync(const SyncArgs& args, char** error_message) {
    if (args.src == nullptr && args.common == nullptr)
        return new_file(args, error_message);
    if (args.src && args.common)
        return existing_file(args, error_message);
    else
        return no_common(args, error_message);
}

LvcConflictArray operation::sync(Paths& src_paths, Paths& dest_paths, char** error_message) {
    const std::string           workspace = io::content(src_paths.current, 0, error_message);
    const std::filesystem::path operation = src_paths.local / workspace;
    operation::create_folder(operation, operation::type::sync, error_message);

    std::filesystem::path    src_workspace       = workspace_path(src_paths.workspace, workspace);
    std::filesystem::path    dest_workspace      = workspace_path(dest_paths.workspace, workspace);
    std::vector<std::string> src_version_vector  = io::content_lines(src_workspace, 0, error_message);
    std::vector<std::string> dest_version_vector = io::content_lines(dest_workspace, 0, error_message);
    std::string              src_id              = src_version_vector[0];
    std::string              dest_id             = dest_version_vector[0];
    std::string              common_id           = latest_common_version(src_version_vector, dest_version_vector);

    std::vector<object::info> dest = version::all_objects(dest_paths.object, dest_id, {}, error_message);
    std::unordered_map<std::filesystem::path, object::info> src =
        version::map_all_objects(src_paths, src_id, {}, error_message);
    std::unordered_map<std::filesystem::path, object::info> common =
        version::map_all_objects(src_paths, common_id, {}, error_message);

    SyncArgs         args = {&src_paths, &dest_paths, nullptr, nullptr, nullptr};
    LvcConflictArray out  = {0};

    for (object::info& object : dest) {
        args.dest = &object;

        std::unordered_map<std::filesystem::path, object::info>::iterator          src_it    = src.find(object.path);
        std::unordered_map<std::filesystem::__cxx11::path, object::info>::iterator common_it = common.find(object.path);

        args.src    = src_it != src.end() ? &src_it->second : nullptr;
        args.common = common_it != common.end() ? &common_it->second : nullptr;

        LvcConflict conflict = sync(args, error_message);
        if (conflict.relative_path)
            insert_conflict(conflict, out, error_message);
    }

    // on success
    std::filesystem::remove_all(operation);
    return {};
}