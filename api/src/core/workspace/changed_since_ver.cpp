#include "core.hpp"

bool workspace::changed_since_ver(
    Paths& paths, const std::vector<object::info>& objects_version, char** error_message) {
    std::vector<object::info> objects_working_dir = workspace::all_objects(paths, error_message);

    std::unordered_set<std::string> version_ids_and_paths;
    version_ids_and_paths.reserve(objects_version.size());
    for (const object::info& object : objects_version) {
        version_ids_and_paths.emplace(object.id);
        version_ids_and_paths.emplace(object.path.string());
    }

    for (const object::info& object : objects_working_dir)
        if (!version_ids_and_paths.contains(object.id) || !version_ids_and_paths.contains(object.path.string()))
            return true;
    return false;
}