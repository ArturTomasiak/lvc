#include <core.hpp>

std::vector<std::string> version::checkout(Paths& paths, char** error_message) {
    const std::string              workspace_name    = io::content(paths.current, 0, error_message);
    const std::filesystem::path    workspace         = paths.workspace / workspace_name;
    const std::vector<std::string> workspace_content = io::content_lines(workspace, 1, error_message);
    const std::string              version           = workspace_content[1];

    std::vector<object::info> objects_version     = version::all_objects(paths.object, version, {}, error_message);
    std::vector<object::info> objects_working_dir = workspace::all_objects(paths, error_message);

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