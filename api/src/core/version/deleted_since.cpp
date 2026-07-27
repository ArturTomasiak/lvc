#include <core.hpp>

std::vector<Object> version::deleted_since(std::filesystem::path object_dir, std::filesystem::path working_directory, std::string id) {
    std::vector<Object> version = version::all_objects(object_dir, working_directory, id);
    std::vector<Object> working = workspace::all_objects(working_directory);

    std::erase_if(version, [&](const Object& version_object) {
        return std::ranges::any_of(
            working, [&](const Object& working_object) {
                return version_object.id == working_object.id;
            });
    });

    return version;
}