#include <internal.hpp>

static void get_all_objects(std::unordered_set<std::string>& objects, std::string tree_id) {
    
}

std::unordered_set<std::string> get::objects_in_version(std::filesystem::path object, std::string version) {
    std::vector<std::string> version_content = get::file_content_lines(object / version);
    std::unordered_set<std::string> objects;
    get_all_objects(objects, version_content[VERSION_ROOT_TREE]);
    return objects;
}