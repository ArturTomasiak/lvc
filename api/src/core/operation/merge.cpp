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
    Paths& paths, const std::string& src_workspace_name, const std::string& dest_workspace_name, char** error_message) {
    LvcConflictArray* out                      = NULL;
    out                                        = static_cast<LvcConflictArray*>(malloc(sizeof(LvcConflictArray)));
    out->len                                   = 0;
    out->alloc                                 = 10;
    out->paths                                 = static_cast<char**>(malloc(out->alloc * sizeof(char*)));
    const std::filesystem::path src_workspace  = paths.workspace / src_workspace_name;
    const std::filesystem::path dest_workspace = paths.workspace / dest_workspace_name;
    const std::filesystem::path conflict_path  = paths.local / dest_workspace_name / filename_conflict;
    const Lines                 src_versions   = io::content_lines(src_workspace, 1, 1, error_message);
    const Lines                 dest_versions  = io::content_lines(dest_workspace, 1, 1, error_message);
    const std::string           src_id         = src_versions.empty() ? "" : src_versions[0];
    const std::string           dest_id        = dest_versions.empty() ? "" : dest_versions[0];
    std::string                 common_id      = src_versions.front();
    bool                        common_exists  = false;
    ERRORCHECK(error_message, out);

    if (!conflicts_handled(paths.root, conflict_path, error_message))
        error_message_creator("Unhandled conflicts. Use lvc conflict for the list of conflicts.", error_message);
    ERRORCHECK(error_message, out);

    for (const auto& src_version : src_versions) {
        if (std::find(dest_versions.begin(), dest_versions.end(), src_version) != dest_versions.end()) {
            common_id     = src_version;
            common_exists = true;
            break;
        }
    }

    const std::vector<object::info> src_objects = version::all_objects(paths.object, src_id, {}, error_message);
    std::unordered_map<std::filesystem::path, object::info> dest_objects =
        version::map_all_objects(paths, dest_id, {}, error_message);
    const std::unordered_map<std::filesystem::path, object::info> common_objects =
        version::map_all_objects(paths, common_id, {}, error_message);
    ERRORCHECK(error_message, out);

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

        if (src_info.id == dest_info->id)
            continue;

        Lines common;
        if (common_info != nullptr && common_exists == true)
            common = io::content_lines(paths.object / common_info->id, 1, 1, error_message);
        const Lines src  = io::content_lines(paths.object / src_info.id, 1, 1, error_message);
        const Lines dest = io::content_lines(paths.object / dest_info->id, 1, 1, error_message);
        ERRORCHECK(error_message, out);

        const bool conflict = merge3(paths.root / src_info.path, common, src, dest);
        if (conflict)
            add_path(out, src_info.path, error_message);
        else
            std::cout << "MERGED:  " << src_info.path << '\n';

        if (dest_info != nullptr)
            dest_objects.erase(src_info.path);
    }

    for (auto& it : dest_objects) {
        object::info& info = it.second;
        if (info.type == object::type::tree)
            continue;
        const auto        common_it = common_objects.find(info.path);
        const std::string buffer    = io::content(paths.object / info.id, 1, 1, error_message);
        ERRORCHECK(error_message, out);
        uint32_t          pos       = buffer.find('\n');
        if (common_it != common_objects.end()) {
            if (info.id == common_it->second.id)
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

    operation::create(paths, dest_workspace_name, src_workspace_name, operation::type::merge, out, error_message);
    
    return out;
}