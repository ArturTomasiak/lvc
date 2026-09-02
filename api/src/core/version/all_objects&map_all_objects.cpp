#include <core.hpp>

static void get_all_paths(
    const std::filesystem::path& object_folder, std::vector<object::info>& out, const std::string tree_id, const std::filesystem::path current,
    std::unordered_set<std::string_view>& ignore, char** error_message) {
    std::vector<std::string> content = io::content_lines(object_folder / tree_id, 1, error_message);
    for (size_t i = 1; i < content.size(); i++) {
        object::info       object;
        const std::string& line = content[i];

        size_t      position = line.find(' ');
        std::string type     = line.substr(0, position);

        object.type = type == TYPE_TREE ? object::type::tree : object::type::blob;

        object.id        = line.substr(position + 1);
        position         = object.id.find(' ');
        std::string name = object.id.substr(position + 1);
        object.id        = object.id.substr(0, position);
        object.path      = current / name;

        if (type == TYPE_TREE) get_all_paths(object_folder, out, object.id, object.path, ignore, error_message);
        if (!ignore.contains(object.path.string())) out.push_back(object);
    }
}

std::vector<object::info>
version::all_objects(std::filesystem::path object_dir, std::string id, std::unordered_set<std::string_view> ignore, char** error_message) {
    if (id.empty()) return {};
    std::vector<std::string> version_content = io::content_lines(object_dir / id, 1, error_message);
    if (version_content.empty()) return {};
    std::vector<object::info> out;
    out.reserve(PREALLOCATE);
    get_all_paths(object_dir, out, version_content[VERSION_ROOT_TREE], "", ignore, error_message);
    return out;
}

static void map_get_all_paths(
    const std::filesystem::path& object_folder, std::unordered_map<std::filesystem::path, object::info>& out, const std::string tree_id,
    const std::filesystem::path current, std::unordered_set<std::string_view>& ignore, char** error_message) {
    std::vector<std::string> content = io::content_lines(object_folder / tree_id, 1, error_message);
    for (size_t i = 1; i < content.size(); i++) {
        object::info       object;
        const std::string& line = content[i];

        size_t      position = line.find(' ');
        std::string type     = line.substr(0, position);

        object.type = type == TYPE_TREE ? object::type::tree : object::type::blob;

        object.id        = line.substr(position + 1);
        position         = object.id.find(' ');
        std::string name = object.id.substr(position + 1);
        object.id        = object.id.substr(0, position);
        object.path      = current / name;

        if (type == TYPE_TREE) map_get_all_paths(object_folder, out, object.id, object.path, ignore, error_message);
        if (!ignore.contains(object.path.string())) out.insert({object.path, object});
    }
}

std::unordered_map<std::filesystem::path, object::info>
version::map_all_objects(std::filesystem::path object_dir, std::string id, std::unordered_set<std::string_view> ignore, char** error_message) {
    if (id.empty()) return {};
    std::vector<std::string> version_content = io::content_lines(object_dir / id, 1, error_message);
    if (version_content.empty()) return {};
    std::unordered_map<std::filesystem::path, object::info> out;
    out.reserve(PREALLOCATE);
    map_get_all_paths(object_dir, out, version_content[VERSION_ROOT_TREE], "", ignore, error_message);
    return out;
}