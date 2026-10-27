#include <core.hpp>

static void add_path(LvcConflictArray* out, const std::filesystem::path& path, char** error_message) {
    if (out->len == out->alloc) {
        out->alloc *= 2;
        out->paths = (char**)realloc(out, out->alloc * sizeof(char*));
        CHECKALLOC(out->paths, error_message);
    }
    size_t len = charplen(path.c_str());
    char*  str = (char*)malloc(len + 1);
    CHECKALLOC(str, error_message);
    out->paths[out->len] = str;
    out->len++;
}

static void existance_conflict(
    Paths& paths, LvcConflictArray* out, const std::filesystem::path& dest, const std::string& buffer, bool ours,
    char** error_message) {
    std::ofstream file(paths.root / dest, std::ios::binary | std::ios::trunc | std::ios::out);
    file << ours_label << '\n';
    if (ours)
        file << buffer << '\n';
    file << ours_label << '\n';
    if (!ours)
        file << buffer << '\n';
    add_path(out, dest, error_message);
    file.close();
}

LvcConflictArray* operation::merge(
    Paths& paths, const std::string& src_workspace_name, const std::string& dest_workspace_name, const char* author,
    char** error_message) {
    LvcConflictArray*           out            = NULL;
    const std::filesystem::path src_workspace  = paths.workspace / src_workspace_name;
    const std::filesystem::path dest_workspace = paths.workspace / dest_workspace_name;
    std::string                 operation;

    const operation::type     type          = operation::ongoing(paths, dest_workspace_name, operation, error_message);
    const Lines               src_versions  = io::content_lines(src_workspace, 0, 1, error_message);
    const Lines               dest_versions = io::content_lines(dest_workspace, 0, 1, error_message);
    const std::string         src_id        = src_versions.empty() ? "" : src_versions[0];
    const std::string         dest_id       = dest_versions.empty() ? "" : dest_versions[0];
    std::string               common_id     = src_versions.front();
    bool                      common_exists = false;
    std::vector<object::info> src_objects;
    std::unordered_map<std::filesystem::path, object::info> dest_objects;
    std::unordered_map<std::filesystem::path, object::info> common_objects;

    if (type != operation::type::none && conflicts_handled(paths, dest_workspace_name, error_message) == false) {
        error_message_creator(
            "Unhandled conflicts from previous " + operation +
                " operation. Use lvc conflict for the list of conflicts.",
            error_message);
        return out;
    }

    for (const auto& src_version : src_versions) {
        if (std::find(dest_versions.begin(), dest_versions.end(), src_version) != dest_versions.end()) {
            common_id     = src_version;
            common_exists = true;
            break;
        }
    }

    src_objects    = version::all_objects(paths.object, src_id, {}, error_message);
    dest_objects   = version::map_all_objects(paths, dest_id, {}, error_message);
    common_objects = version::map_all_objects(paths, common_id, {}, error_message);
    out            = (LvcConflictArray*)malloc(sizeof(LvcConflictArray));
    out->len       = 0;
    out->alloc     = 10;

    for (const object::info& src_info : src_objects) {
        const auto          dest_it     = dest_objects.find(src_info.path);
        const auto          common_it   = common_objects.find(src_info.path);
        const object::info* dest_info   = nullptr;
        const object::info* common_info = nullptr;

        if (dest_it != dest_objects.end())
            dest_info = &dest_it->second;
        if (common_it != common_objects.end())
            common_info = &common_it->second;

        if (src_info.type == object::type::tree)
            continue;

        if (dest_info == nullptr) {
            if (common_info != nullptr && src_info.id != common_info->id) {
                const std::string src = io::content(paths.object / src_info.id, 1, 1, error_message);
                existance_conflict(paths, out, src_info.path, src, 1, error_message);
            }
            continue;
        }
        Lines common;
        if (common_info != nullptr && common_exists == true)
            common = io::content_lines(paths.object / common_info->id, 1, 1, error_message);
        const Lines src  = io::content_lines(paths.object / src_info.id, 1, 1, error_message);
        const Lines dest = io::content_lines(paths.object / dest_info->id, 1, 1, error_message);

        const MergeResult state = merge3(paths.root / src_info.path, common, src, dest);
        if (state == MergeResult::conflict) {
            add_path(out, src_info.path, error_message);
        } else if (state == MergeResult::merge) {
            std::cout << "MERGED:  " << src_info.path << '\n';
        }

        if (dest_info != nullptr)
            dest_objects.erase(src_info.path);
    }

    for (auto& it : dest_objects) {
        object::info&     info      = it.second;
        const auto        common_it = common_objects.find(info.path);
        const std::string buffer    = io::content(paths.object / info.id, 1, 1, error_message);
        uint32_t          pos       = buffer.find('\n');
        if (common_it != common_objects.end()) {
            if (info.id == common_id)
                continue;
            existance_conflict(paths, out, info.path, buffer, 0, error_message);
        } else {
            io::file(
                paths.root / info.path, std::ios::binary | std::ios::trunc, buffer.data(), buffer.size(), 0,
                error_message);
            std::cout << "NEW:  " << info.path << '\n';
        }
    }

    for (size_t i = 0; i < out->len; i++) {
        std::cout << "CONFLICT: " << out->paths[i] << '\n';
    }

success:
    std::string message = "insert workspace " + src_workspace_name;
    std::cout << "Successfully inserted " + src_workspace_name;
    version::create(paths, message.c_str(), author, src_workspace, error_message);
    return out;
}