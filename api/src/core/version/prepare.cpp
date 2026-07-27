#include <core.hpp>

LvcError version::prepare(std::filesystem::path lvc, std::vector<std::string> input, char*** prepared) {
    const std::string workspace_name       = content(lvc / NAME_CURRENT, 0);
    const std::filesystem::path workspace  = workspace_path(lvc, workspace_name);
    const std::filesystem::path object_dir = lvc / NAME_OBJECT;

    std::vector<std::string> paths = path_from_input(lvc.parent_path(), input, content_first_line(workspace));
    if (paths.empty())
        return PREPARE_NO_INPUT;

    std::vector<std::string> current = content_lines(lvc / NAME_PREPARE, 0);

    std::unordered_set<std::string> existing;
    existing.reserve(current.size() + paths.size());
    existing.insert(current.begin(), current.end());

    std::erase_if(paths, [&](const std::string& path) {
        return existing.contains(path);
    });

    for (std::string& path : paths) {
        file_create(lvc / NAME_PREPARE, std::ios::binary | std::ios::app, path.data(), path.size(), 0);
        file_create(lvc / NAME_PREPARE, std::ios::binary | std::ios::app, "\n", 1, 0);
        const std::filesystem::path object = object_dir / path;
        if (prepared && std::filesystem::exists(object) && std::filesystem::is_regular_file(object)) {
            path = "REMOVED OBJECT: " + path;
        }
    }

    if (prepared)
        *prepared = strvector_to_charpp(paths);

    return SUCCESS;
}