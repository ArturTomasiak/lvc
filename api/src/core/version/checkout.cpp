#include <core.hpp>

std::vector<std::string> version::checkout(std::filesystem::path lvc, char** error_message) {
    std::string           workspace_name = io::content(lvc / NAME_CURRENT, 0, error_message);
    std::filesystem::path workspace      = workspace_path(lvc / NAME_WORKSPACE, workspace_name);
    std::filesystem::path working_dir    = lvc.parent_path();
    std::string           version        = io::content_first_line(workspace, error_message);

    std::vector<object::info> objects_version     = version::all_objects(lvc / NAME_OBJECT, version, {}, error_message);
    std::vector<object::info> objects_working_dir = workspace::all_objects(working_dir, error_message);

    std::unordered_set<std::string> version_ids_and_paths;
    version_ids_and_paths.reserve(objects_version.size());
    for (const object::info& object : objects_version) {
        version_ids_and_paths.emplace(object.id);
        version_ids_and_paths.emplace(object.path.string());
    }

    std::vector<std::string> result;
    result.reserve(objects_working_dir.size());

    for (const object::info& object : objects_working_dir) {
        if (!version_ids_and_paths.contains(object.id) || !version_ids_and_paths.contains(object.path.string()))
            result.push_back(std::move(object.path));
    }

    return result;
}