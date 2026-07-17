#include <internal.hpp>

static void get_all_objects(std::filesystem::path object, std::unordered_set<std::string>& out, std::string tree_id) {
    std::vector<std::string> content = get::file_content_lines(object / tree_id, 1);
    for (const std::string& line : content) {
        std::size_t position = line.find(' ');
        if (position == std::string::npos)
            continue;
        std::string type = line.substr(0, position);
        std::string id   = line.substr(position + 1);
        if (type == TYPE_TREE)
            get_all_objects(object, out, id);
        out.insert(id);
    }
}

std::unordered_set<std::string> get::objects_in_version(std::filesystem::path object, std::string version) {
    std::vector<std::string> version_content = get::file_content_lines(object / version, 1);
    std::unordered_set<std::string> out;
    get_all_objects(object, out, version_content[VERSION_ROOT_TREE]);
    return out;
}