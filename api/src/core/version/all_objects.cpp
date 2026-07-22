#include <core.hpp>

static void get_all_paths(const std::filesystem::path& object_folder, std::vector<Object>& out, const std::string& tree_id, const std::filesystem::path& current) {
    std::vector<std::string> content = file::content_lines(object_folder / tree_id, 1);
    for (uint64_t i = 1; i < content.size(); i++) {
        Object object;
        const std::string& line = content[i];

        uint64_t position = line.find(' ');
        std::string type  = line.substr(0, position);

        object.type = type == "tree" ? TREE : BLOB;

        object.id        = line.substr(position + 1);
        position         = object.id.find(' ');
        std::string name = object.id.substr(position + 1);
        object.id        = object.id.substr(0, position);
        object.path      = current / name;

        if (type == TYPE_TREE)
            get_all_paths(object_folder, out, object.id, object.path);
        else
            out.push_back(object);
    }
}

std::vector<Object> all_paths(std::filesystem::path& object, std::filesystem::path working_directory, std::string id) {
    std::vector<std::string> version_content = file::content_lines(object / id, 1);
    std::vector<Object> out;
    out.reserve(PREALLOCATE);
    get_all_paths(object, out, version_content[VERSION_ROOT_TREE], working_directory);
    for (Object& object : out)
        object.path = object.path.lexically_relative(working_directory);
        
    return out;
}