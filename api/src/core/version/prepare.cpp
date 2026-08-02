#include <core.hpp>

LvcError version::prepare_reset(std::filesystem::path lvc) {
    std::error_code error;
    bool deleted = std::filesystem::remove(lvc / NAME_STATUS, error);
    if (!deleted || error)
        return PREPARE_RESET_ERROR;
    return SUCCESS;
}

LvcError version::prepare(std::filesystem::path lvc, std::vector<std::string> input, char*** prepared) {
    const std::string workspace_name       = io::content(lvc / NAME_CURRENT, 0);
    const std::filesystem::path workspace  = workspace_path(lvc, workspace_name);
    const std::filesystem::path object_dir = lvc / NAME_OBJECT;

    LvcError err;
    std::vector<std::string> paths = path_from_input(lvc.parent_path(), input, io::content_first_line(workspace), err);
    if (err) return err;
    
    if (paths.empty())
        return PREPARE_NO_INPUT;

    std::vector<std::string> current = io::content_lines(lvc / NAME_PREPARE, 0);

    for (const std::string& path : paths)
        if (std::find(current.begin(), current.end(), path) == current.end())
            current.push_back(path);

    for (std::string& path : paths) {
        io::file(lvc / NAME_PREPARE, std::ios::binary | std::ios::app, path.data(), path.size(), 0);
        io::file(lvc / NAME_PREPARE, std::ios::binary | std::ios::app, "\n", 1, 0);
        const std::filesystem::path object = object_dir / path;
        if (std::filesystem::exists(object) && std::filesystem::is_regular_file(object))
            path = "REMOVED OBJECT: " + path;
    }

    if (prepared)
        *prepared = strvector_to_charpp(paths);

    return SUCCESS;
}