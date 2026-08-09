#include <core.hpp>

LvcError version::prepare_reset(std::filesystem::path lvc) {
    std::error_code error;
    bool deleted = std::filesystem::remove(lvc / NAME_PREPARE, error);
    if (!deleted || error)
        return PREPARE_RESET_ERROR;
    return SUCCESS;
}

LvcError version::prepare(std::filesystem::path lvc, std::vector<std::string> input_raw, char*** prepared) {
    const std::string workspace_name        = io::content(lvc / NAME_CURRENT, 0);
    const std::filesystem::path workspace   = workspace_path(lvc / NAME_WORKSPACE, workspace_name);
    const std::filesystem::path object_dir  = lvc / NAME_OBJECT;
    const std::filesystem::path working_dir = lvc.parent_path();
    const std::string version_id            = io::content_first_line(workspace);

    LvcError err;
    std::vector<std::string> ignore_raw = io::content_lines(working_dir / NAME_IGNORE, 0);
    std::vector<std::string> ignore     = path_from_input(working_dir, ignore_raw, version_id, err);
    if (err) return err;

    std::vector<std::string> input = path_from_input(working_dir, input_raw, version_id, err);
    if (err) return err;
    if (input.empty())
        return PREPARE_NO_INPUT;

    std::vector<std::string> current = io::content_lines(lvc / NAME_PREPARE, 0);

    std::unordered_set<std::string> current_entries;
    current_entries.reserve(current.size());
    for (const std::string& path : current)
        current_entries.insert(path);

    for (const std::string& path : input)
        if (!current_entries.contains(path))
            current.push_back(path);

    std::unordered_set<std::string> ignore_entries;
    ignore_entries.reserve(ignore.size());
    for (const std::string& entry : ignore)
        ignore_entries.insert(entry);

    std::erase_if(current, [&ignore_entries](const std::string& entry) {return ignore_entries.contains(entry);});

    std::string buffer;

    for (std::string& path : current) {
        const std::filesystem::path object = object_dir / path;
        path += "\n";
        buffer += path;
        if (std::filesystem::is_regular_file(object))
            path = "REMOVED OBJECT: " + path;
    }

    io::file(lvc / NAME_PREPARE, std::ios::binary, buffer.data(), buffer.size(), 0);

    if (prepared)
        *prepared = strvector_to_charpp(current);

    return SUCCESS;
}