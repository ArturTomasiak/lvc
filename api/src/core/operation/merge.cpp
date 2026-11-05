#include <core.hpp>

static void add_path(LvcConflictArray* out, const std::filesystem::path& path, char** error_message) {
    if (out->len == out->alloc) {
        out->alloc *= 2;
        out->paths = static_cast<char**>(realloc(out, out->alloc * sizeof(char*)));
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
    std::ofstream file(paths.root / dest, std::ios::binary | std::ios::trunc | std::ios::out);
    file << ours_label << '\n';
    if (ours)
        file << buffer << '\n';
    file << theirs_label << '\n';
    if (!ours)
        file << buffer << '\n';
    file << end_label << '\n';
    add_path(out, dest, error_message);
    file.close();
}

LvcConflictArray* operation::merge(
    Paths& dest_paths, const std::string& src_workspace_name, const std::string& dest_workspace_name,
    const char* src_lvc, char** error_message) {
    Paths             src_paths(src_lvc == 0 ? dest_paths.lvc.c_str() : src_lvc, error_message);
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

    const std::vector<object::info> dest_objects = version::all_objects(dest_paths.object, dest_id, {}, error_message);
    const std::vector<object::info> src_objects  = version::all_objects(src_paths.object, src_id, {}, error_message);
    std::unordered_map<std::filesystem::path, object::info> dest_map;
    std::unordered_map<std::filesystem::path, object::info> src_map;
    std::unordered_map<std::filesystem::path, object::info> common_map =
        common_id.empty() ? std::unordered_map<std::filesystem::path, object::info>{}
                          : version::map_all_objects(dest_paths, common_id, {}, error_message);
    dest_map.reserve(dest_objects.size());
    src_map.reserve(src_objects.size());
    for (const object::info& object : dest_objects)
        dest_map.insert({object.path, object});
    for (const object::info& object : src_objects)
        src_map.insert({object.path, object});
    CHECKERROR(error_message, out);

    size_t d = 0, s = 0;
    bool   dend = false, send = false;
    while (!dend && !send) {
        const object::info&         dest            = dest_objects[d];
        const object::info&         src             = src_objects[s];
        const std::filesystem::path src_object_path = src_paths.object / src.id;

        if (remote_src) {
            const std::filesystem::path src_object_dest_path = dest_paths.object / src.id;
            if (!std::filesystem::is_regular_file(src_object_dest_path))
                std::filesystem::copy(src_object_path, src_object_dest_path, ec);
        }

        if (dest.path == src.path) {
            dest_objects.size() - 1 > d ? d++ : dend = true;
            src_objects.size() - 1 > s ? s++ : send = true;
            const auto    common_it = common_map.find(dest.path);
            object::info* common    = nullptr;
            if (common_it != common_map.end())
                common = &common_it->second;

            if (dest.id == src.id) {
            } else {
                up_to_date    = false;
                bool conflict = merge3(
                    dest.path,
                    common == nullptr ? Lines{}
                                      : io::content_lines(dest_paths.root / common->path, 1, 1, error_message),
                    io::content_lines(dest_paths.root / dest.path, 1, 1, error_message),
                    io::content_lines(src_paths.root / src.path, 1, 1, error_message));
                if (conflict)
                    add_path(out, dest.path, error_message);
                else
                    std::cout << "MERGED: " << dest.path << '\n';
            }
        } else {
            const auto dest_it = dest_map.find(src.path);
            const auto src_it  = src_map.find(dest.path);
            if (dest_it == dest_map.end()) {
                up_to_date = false;
                src_objects.size() - 1 > s ? s++ : send = true;
                const auto common_it = common_map.find(src.path);
                if (common_it == common_map.end()) {
                    const std::string buffer = io::content(src_object_path, 1, 1, error_message);
                    io::file(
                        dest_paths.root / src.path, std::ios::binary | std::ios::trunc, buffer.data(), buffer.size(), 0,
                        error_message);
                } else if (common_it->second.id == src.id) {
                } else {
                    existance_conflict(
                        dest_paths, out, src.path, io::content(src_paths.root / src.path, 1, 1, error_message), false,
                        error_message);
                }
            }
            if (src_it == src_map.end()) {
                up_to_date = false;
                dest_objects.size() - 1 > d ? d++ : dend = true;
                const auto common_it = common_map.find(dest.path);
                if (common_it == common_map.end()) {
                } else if (common_it->second.id == dest.id) {
                    std::filesystem::remove(dest_paths.root / src.path, ec);
                } else {
                    existance_conflict(
                        dest_paths, out, dest.path, io::content(dest_paths.root / dest.path, 1, 1, error_message), true,
                        error_message);
                }
            }
        }
    }

    for (size_t i = 0; i < out->len; i++) {
        std::cout << "CONFLICT: " << out->paths[i] << '\n';
    }

    if (src_workspace_name == dest_workspace_name) {
        std::string workspace;
        for (const std::string& version : src_versions) {
            if (version == common_id)
                break;
            workspace += version + '\n';
        }
        for (const std::string& version : dest_versions)
            workspace += version + '\n';
        insert_pattern(workspace, TYPE_WORKSPACE, dest_category.empty() ? 0 : dest_category.c_str());
        io::file(
            dest_workspace, std::ios::binary | std::ios::trunc, workspace.data(), workspace.size(), 1, error_message);
    }

    if (up_to_date) {
        std::cout << "Already up to date with " << src_workspace_name << '\n';
    } else {
        operation::create(
            dest_paths, dest_workspace_name, src_workspace_name, operation::type::merge, out, error_message);
    }

    return out;
}