#include <core.hpp>

void version::prepare_reset(Paths& paths, char** error_message) {
    const std::string           workspace_name = io::content(paths.current, 0, error_message);
    const std::filesystem::path prepare_dir    = prepare_path(paths, workspace_name);
    std::error_code             error;
    bool                        deleted = std::filesystem::remove(prepare_dir, error);
    if (!deleted || error)
        error_message_creator("Could not reset prepare", error_message);
}

void version::prepare(Paths& paths, std::vector<std::string> input_raw, char*** prepared, char** error_message) {
    if (input_raw.empty()) {
        error_message_creator("No input recieved", error_message);
        return;
    }

    const std::string           workspace_name = io::content(paths.current, 0, error_message);
    const std::filesystem::path workspace      = workspace_path(paths.workspace, workspace_name);
    const std::filesystem::path prepare_dir    = prepare_path(paths, workspace_name);
    const std::string           version_id     = io::content_first_line(workspace, error_message);

    std::vector<std::string> ignore_raw = io::content_lines(paths.ignore, 0, error_message);
    std::vector<std::string> ignore     = paths.resolve(ignore_raw, version_id, 1, 1, error_message);
    if (*error_message)
        return;

    std::vector<std::string> input = paths.resolve(input_raw, version_id, 1, 1, error_message);
    if (*error_message)
        return;
    if (input.empty())
        return;

    std::vector<std::string> current = io::content_lines(prepare_dir, 0, error_message);

    std::unordered_set<std::string_view> current_entries;
    current_entries.reserve(current.size());
    for (const std::string& path : current)
        current_entries.insert(path);

    for (const std::string& path : input)
        if (!current_entries.contains(path))
            current.push_back(path);

    std::unordered_set<std::string_view> ignore_entries;
    ignore_entries.reserve(ignore.size());
    for (const std::string& entry : ignore)
        ignore_entries.insert(entry);

    std::erase_if(current, [&ignore_entries](const std::string& entry) { return ignore_entries.contains(entry); });

    std::unordered_set<std::string> version = version::all_ids(paths, version_id, ignore_entries, error_message);
    std::erase_if(current, [&version, &paths, &error_message](const std::string& entry) {
        std::string buffer = io::content(paths.root / entry, 0, error_message);
        insert_pattern(buffer, TYPE_BLOB);
        char id[65];
        sha256(buffer.c_str(), buffer.size(), id);
        return version.contains(id);
    });

    std::string buffer;
    for (std::string& path : current)
        buffer += path + "\n";

    insert_pattern(buffer, TYPE_PREPARE);
    io::file(prepare_dir, std::ios::binary | std::ios::trunc, buffer.data(), buffer.size(), 1, error_message);

    if (prepared)
        *prepared = strvector_to_charpp(current);
}