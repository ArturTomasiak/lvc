#include <core.hpp>

static void add_path(LvcConflictArray* out, const std::filesystem::path& path, char** error_message) {
    if (out->len == out->alloc) {
        out->alloc *= 2;
        out->paths = static_cast<char**>(realloc(out->paths, out->alloc * sizeof(char*)));
        CHECKALLOC(out->paths, error_message);
    }
    size_t len = charplen(path.c_str()) + 1;
    char*  str = static_cast<char*>(malloc(len));
    CHECKALLOC(str, error_message);
    memcpy(str, path.c_str(), len);
    out->paths[out->len] = str;
    out->len++;
}

static void existance_conflict(
    Paths& paths, LvcConflictArray* out, const std::filesystem::path& dest, const std::string& buffer, bool ours,
    char** error_message) {
    const Pending pending = {.type = PendingType::rename, .src = paths.get_tmp_file(), .dest = paths.root / dest};
    std::ofstream file(pending.src, std::ios::binary | std::ios::trunc | std::ios::out);
    file << ours_label << '\n';
    if (ours)
        file << buffer << '\n';
    file << theirs_label << '\n';
    if (!ours)
        file << buffer << '\n';
    file << end_label << '\n';
    file.close();
    add_path(out, dest, error_message);
    paths.move_pending(pending);
}

LvcConflictArray* operation::merge(
    Paths& dest_paths, const std::string& src_workspace_name, const std::string& dest_workspace_name,
    const char* src_lvc, char** error_message) {
    Paths             src_paths(src_lvc == 0 ? dest_paths.lvc.c_str() : src_lvc, false, error_message);
    LvcConflictArray* out                      = NULL;
    out                                        = static_cast<LvcConflictArray*>(malloc(sizeof(LvcConflictArray)));
    out->len                                   = 0;
    out->alloc                                 = 10;
    out->paths                                 = static_cast<char**>(malloc(out->alloc * sizeof(char*)));
    const std::filesystem::path src_workspace  = src_paths.workspace / src_workspace_name;
    const std::filesystem::path dest_workspace = dest_paths.workspace / dest_workspace_name;
    const std::filesystem::path conflict_path  = dest_paths.local / dest_workspace_name / filename_conflict;
    const Lines                 src_versions   = io::content_lines(src_workspace, 1, 1, error_message);
    Lines                       dest_versions  = io::content_lines(dest_workspace, 1, 0, error_message);
    const std::string           dest_category  = category::get(dest_versions[0]);
    dest_versions.erase(dest_versions.begin());
    const std::string src_id     = src_versions.empty() ? "" : src_versions[0];
    const std::string dest_id    = dest_versions.empty() ? "" : dest_versions[0];
    bool              up_to_date = true;
    bool              remote_src = dest_paths.root != src_paths.root;
    std::string       common_id;
    std::error_code   ec;
    CHECKERROR(error_message, out);

    if (!conflicts_handled(dest_paths.root, conflict_path, error_message)) [[unlikely]] {
        error_message_creator("Unhandled conflicts. Use lvc conflict for the list of conflicts.", error_message);
        return out;
    }

    for (const auto& src_version : src_versions) {
        if (std::find(dest_versions.begin(), dest_versions.end(), src_version) != dest_versions.end()) {
            common_id = src_version;
            break;
        }
    }

    std::unordered_map<std::filesystem::path, object::info> dest_map;
    std::unordered_map<std::filesystem::path, object::info> src_map;
    std::unordered_map<std::filesystem::path, object::info> common_map =
        common_id.empty() ? std::unordered_map<std::filesystem::path, object::info>{}
                          : version::map_all_objects(dest_paths, common_id, {}, error_message);
    std::vector<std::filesystem::path> all_paths;

    {
        const std::vector<object::info> dest_objects =
            version::all_objects(dest_paths.object, dest_id, {}, error_message);
        const std::vector<object::info> src_objects = version::all_objects(src_paths.object, src_id, {}, error_message);

        if (workspace::changed_since_ver(dest_paths, dest_objects, error_message)) {
            error_message_creator("Unsaved changes. Create a new version before merging.", error_message);
            return out;
        }

        dest_map.reserve(dest_objects.size());
        src_map.reserve(src_objects.size());
        all_paths.reserve(dest_objects.size() + src_objects.size());

        for (const object::info& object : dest_objects) {
            dest_map.insert({object.path, object});
            all_paths.emplace_back(std::move(object.path));
        }
        for (const object::info& object : src_objects) {
            src_map.insert({object.path, object});
            if (!dest_map.contains(object.path))
                all_paths.emplace_back(std::move(object.path));
        }
    }
    CHECKERROR(error_message, out);

    for (const std::filesystem::path& path : all_paths) {
        const auto dest_it   = dest_map.find(path);
        const auto src_it    = src_map.find(path);
        const auto common_it = common_map.find(path);

        const object::info* dest   = dest_it == dest_map.end() ? nullptr : &dest_it->second;
        const object::info* src    = src_it == src_map.end() ? nullptr : &src_it->second;
        const object::info* common = common_it == common_map.end() ? nullptr : &common_it->second;

        const bool dest_tree = dest ? dest->type == object::type::tree : false;
        const bool src_tree  = src ? src->type == object::type::tree : false;

        if (remote_src && src) {
            const Pending pending = {
                .type = PendingType::rename, .src = dest_paths.get_tmp_file(), .dest = dest_paths.object / src->id};
            if (!std::filesystem::is_regular_file(pending.dest)) {
                std::filesystem::copy(src_paths.object / src->id, pending.src, ec);
                dest_paths.move_pending(pending);
            }
        }

        if (dest && src && !dest_tree && !src_tree) {
            if (dest->id == src->id)
                continue;
            up_to_date            = false;
            const Pending pending = {
                .type = PendingType::rename, .src = dest_paths.get_tmp_file(), .dest = dest_paths.root / dest->path};
            bool conflict = merge3(
                pending.src,
                common == nullptr ? Lines{} : io::content_lines(dest_paths.object / common->id, 1, 1, error_message),
                io::content_lines(dest_paths.object / dest->id, 1, 1, error_message),
                io::content_lines(src_paths.object / src->id, 1, 1, error_message));
            dest_paths.move_pending(pending);
            if (conflict)
                add_path(out, dest->path, error_message);
            else
                std::cout << "MERGED: " << dest->path << '\n';
        }

        else if (dest && !dest_tree) {
            if (common) {
                up_to_date = false;
                if (dest->id == common->id) {
                    dest_paths.move_pending({.type = PendingType::remove, .dest = dest_paths.root / dest->path});
                    std::cout << "REMOVED: " << dest->path << '\n';
                } else
                    existance_conflict(
                        dest_paths, out, dest->path, io::content(dest_paths.object / dest->id, 1, 1, error_message), 1,
                        error_message);
            }
        }

        else if (src && !src_tree) {
            if (common) {
                if (src->id != common->id) {
                    up_to_date = false;
                    existance_conflict(
                        dest_paths, out, src->path, io::content(src_paths.object / src->id, 1, 1, error_message), 0,
                        error_message);
                }
            } else {
                up_to_date            = false;
                const Pending pending = {
                    .type = PendingType::rename, .src = dest_paths.get_tmp_file(), .dest = dest_paths.root / src->path};
                const std::string buffer = io::content(src_paths.object / src->id, 1, 1, error_message);

                io::file(
                    pending.src, std::ios::binary | std::ios::trunc, buffer.c_str(), buffer.size(), 0, error_message);

                dest_paths.move_pending(
                    {.type = PendingType::create_directories_conditional, .dest = pending.dest.parent_path()});
                dest_paths.move_pending(pending);

                std::cout << "NEW: " << src->path << '\n';
            }
        }
    }

    if (up_to_date) {
        std::cout << "Already up to date with " << src_workspace_name << '\n';
        return out;
    }

    const Pending operation = {
        .type = PendingType::create_directory_conditional, .dest = dest_paths.local / dest_workspace_name};
    const Pending merge = {
        .type = PendingType::rename, .src = dest_paths.get_tmp_file(), .dest = operation.dest / filename_merge};
    const Pending conflict = {
        .type = PendingType::rename, .src = dest_paths.get_tmp_file(), .dest = operation.dest / filename_conflict};
    dest_paths.move_pending(operation);

    const std::string merge_buffer = src_workspace_name + '\n' + src_id;
    io::file(merge.src, std::ios::binary | std::ios::trunc, merge_buffer.data(), merge_buffer.size(), 0, error_message);
    dest_paths.move_pending(merge);

    if (out->len != 0) {
        std::ofstream conflict_stream(conflict.src, std::ios::binary | std::ios::trunc | std::ios::out);
        for (size_t i = 0; i < out->len; i++) {
            conflict_stream << out->paths[i];
            conflict_stream << '\n';
            std::cout << "CONFLICT: " << out->paths[i] << '\n';
        }
        conflict_stream.close();
        dest_paths.move_pending(conflict);
    }

    return out;
}