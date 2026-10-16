#include <core.hpp>

LvcConflictArray* operation::insert(
    Paths& paths, const std::string& src_workspace_name, const std::string& dest_workspace_name, const char* author,
    char** error_message) {
    LvcConflictArray*     out = NULL;
    std::filesystem::path src_workspace;
    std::filesystem::path dest_workspace;
    std::string           operation;

    if (workspace::is_inactive(paths, src_workspace_name, src_workspace)) {
        error_message_creator(src_workspace_name + IS_INACTIVE_STRING, error_message);
        return out;
    }
    if (workspace::is_inactive(paths, dest_workspace_name, dest_workspace)) {
        error_message_creator(dest_workspace_name + IS_INACTIVE_STRING, error_message);
        return out;
    }

    const Lines               src_versions  = io::content_lines(src_workspace, 0, error_message);
    const Lines               dest_versions = io::content_lines(dest_workspace, 0, error_message);
    const std::string         src_id        = src_versions[0];
    const std::string         dest_id       = dest_versions[0];
    std::string               common_id     = src_versions.front();
    bool                      common_exists = false;
    operation::type           type          = operation::ongoing(paths, src_workspace_name, operation, error_message);
    std::vector<MergeResult>  result;
    std::vector<object::info> src_objects;
    std::unordered_map<std::filesystem::path, object::info> dest_objects;
    std::unordered_map<std::filesystem::path, object::info> common_objects;

    switch (type) {
        case operation::type::none:
            break;
        case operation::type::insert:
            if (operation::conflicts_handled(paths, src_workspace_name, error_message)) {
                goto success;
            } else {
                std::cout << "Resolve conflicts before insertion";
                return out;
            }
            break;
        default:
            error_message_creator("Ongoing operation: " + operation, error_message);
            return out;
            break;
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

    for (const object::info& src_info : src_objects) {
        const auto          dest_it     = dest_objects.find(src_info.path);
        const auto          common_it   = common_objects.find(src_info.path);
        const object::info* dest_info   = nullptr;
        const object::info* common_info = nullptr;
        const Lines         src         = io::content_lines(paths.object / src_info.id, 1, error_message);
        Lines               dest;
        Lines               common;

        if (dest_it != dest_objects.end())
            dest_info = &dest_it->second;
        if (common_it != common_objects.end())
            common_info = &common_it->second;

        if (dest_info == nullptr && common_info == nullptr)
            continue;
        if (common_info != nullptr || common_exists == true)
            common = io::content_lines(paths.object / common_info->id, 1, error_message);
        dest = io::content_lines(paths.object / dest_info->id, 1, error_message);

        result.emplace_back(merge(common, src, dest, src_info.path));

        if (dest_info != nullptr)
            dest_objects.erase(src_info.path);
    }

    for (auto& it : dest_objects) {
        object::info& info      = it.second;
        const auto    common_it = common_objects.find(info.path);
        std::string   buffer    = io::content(paths.object / info.id, 1, error_message);
        uint32_t      pos       = buffer.find('\n');
        if (common_it != common_objects.end()) {
        } else {
            io::file(
                paths.root / info.path, std::ios::binary | std::ios::trunc, buffer.data() + pos + 1,
                buffer.size() - pos - 1, 0, error_message);
        }
    }

    if (!result.empty()) {
        operation::create(paths, dest_workspace_name, type, result, error_message);

        out        = (LvcConflictArray*)malloc(sizeof(LvcConflictArray));
        out->len   = result.size();
        out->paths = (char**)malloc(out->len * sizeof(char*));
        size_t i   = 0;
        for (const MergeResult& merge : result) {
            size_t path_len = charplen(merge.path.c_str());
            out->paths[i]   = (char*)malloc(path_len + 1);
            memcpy(out->paths[i], merge.path.c_str(), path_len + 1);
            std::cout << "Merge conflict: " << merge.path.c_str() << '\n';
        }
        std::cout << "Retry once conflicts are handled";
        return out;
    }

success:
    std::string message = "insert workspace " + src_workspace_name;
    version::create(paths, message.c_str(), author, src_workspace, error_message);
    std::cout << "Successfully inserted " + src_workspace_name;
    return out;
}