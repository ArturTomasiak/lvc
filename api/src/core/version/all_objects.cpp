#include <core.hpp>

static void get_all_paths(const std::filesystem::path& object_folder, std::vector<Object>& out, const std::string tree_id, const std::filesystem::path current) {
    std::vector<std::string> content = io::content_lines(object_folder / tree_id, 1);
    for (size_t i = 1; i < content.size(); i++) {
        Object object;
        const std::string& line = content[i];

        size_t position = line.find(' ');
        std::string type  = line.substr(0, position);

        object.type = type == TYPE_TREE ? TREE : BLOB;

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

std::vector<Object> version::all_objects(std::filesystem::path object_dir, std::filesystem::path working_directory, std::string id) {
    std::vector<std::string> version_content = io::content_lines(object_dir / id, 1);
    if (version_content.empty())
        return {};
    std::vector<Object> out;
    out.reserve(PREALLOCATE);
    get_all_paths(object_dir, out, version_content[VERSION_ROOT_TREE], working_directory);
    for (Object& object : out)
        object.path = object.path.lexically_relative(working_directory);
        
    return out;
}