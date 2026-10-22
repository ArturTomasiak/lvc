#include <core.hpp>

static void existance_conflict(
    Paths& paths, LvcConflictArray* out, std::filesystem::path dest, const std::string& buffer, bool ours) {
    std::ofstream file(paths.root / dest, std::ios::binary | std::ios::trunc | std::ios::out);
    file << ours_label << '\n';
    if (ours)
        file << buffer << '\n';
    file << ours_label << '\n';
    if (!ours)
        file << buffer << '\n';
    size_t len           = charplen(dest.c_str());
    out->paths           = (char**)realloc(out, (out->len + 1) * sizeof(char*));
    out->paths[out->len] = (char*)malloc(len + 1);
    memcpy(out->paths[out->len], dest.c_str(), len + 1);
    out->len++;
    file.close();
}

LvcConflictArray* operation::insert(
    Paths& paths, const std::string& src_workspace_name, const std::string& dest_workspace_name, const char* author,
    char** error_message) {
    LvcConflictArray*     out = NULL;
    std::filesystem::path src_workspace;
    std::filesystem::path dest_workspace;
    std::string           operation;

    const Lines               src_versions  = io::content_lines(src_workspace, 0, error_message);
    const Lines               dest_versions = io::content_lines(dest_workspace, 0, error_message);
    const std::string         src_id        = src_versions[0];
    const std::string         dest_id       = dest_versions[0];
    std::string               common_id     = src_versions.front();
    bool                      common_exists = false;
    operation::type           type          = operation::ongoing(paths, src_workspace_name, operation, error_message);
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

        if (dest_it != dest_objects.end())
            dest_info = &dest_it->second;
        if (common_it != common_objects.end())
            common_info = &common_it->second;

        if (src_info.type == object::type::tree)
            continue;

        if (dest_info == nullptr) {
            if (common_info != nullptr && src_info.id != common_info->id) {
                const std::string src = io::content(paths.object / src_info.id, 1, error_message);
                existance_conflict(paths, out, paths.root / src_info.path, src, 1);
            }
            continue;
        }
        Lines common;
        if (common_info != nullptr || common_exists == true)
            common = io::content_lines(paths.object / common_info->id, 1, error_message);
        const Lines src  = io::content_lines(paths.object / src_info.id, 1, error_message);
        const Lines dest = io::content_lines(paths.object / dest_info->id, 1, error_message);

        bool conflict = merge(paths.root / src_info.path, common, src, dest);
        if (conflict) {
            size_t len           = charplen(src_info.path.c_str());
            out->paths           = (char**)realloc(out, (out->len + 1) * sizeof(char*));
            out->paths[out->len] = (char*)malloc(len + 1);
            memcpy(out->paths[out->len], src_info.path.c_str(), len + 1);
            out->len++;
        } else {
            std::cout << "MERGED:  " << src_info.path << '\n';
        }

        if (dest_info != nullptr)
            dest_objects.erase(src_info.path);
    }

    for (auto& it : dest_objects) {
        object::info&     info      = it.second;
        const auto        common_it = common_objects.find(info.path);
        const std::string buffer    = io::content(paths.object / info.id, 1, error_message);
        uint32_t          pos       = buffer.find('\n');
        if (common_it != common_objects.end()) {
            if (info.id == common_id)
                continue;
            existance_conflict(paths, out, paths.root / info.path, buffer, 0);
        } else {
            io::file(
                paths.root / info.path, std::ios::binary | std::ios::trunc, buffer.data() + pos + 1,
                buffer.size() - pos - 1, 0, error_message);
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