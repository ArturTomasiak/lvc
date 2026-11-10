#include <core.hpp>

void version::prepare_reset(Paths& paths, char** error_message) {
    const std::string           workspace_name = io::content(paths.current, 0, 0, error_message);
    const std::filesystem::path prepare_dir    = prepare_path(paths, workspace_name);
    paths.move_pending({.type = PendingType::remove, .dest = prepare_dir});
}

void version::prepare(Paths& paths, std::vector<std::string>& input_raw, char*** prepared, char** error_message) {
    if (input_raw.empty()) {
        error_message_creator("No input recieved", error_message);
        return;
    }
    const std::string              workspace_name    = io::content(paths.current, 0, 0, error_message);
    const std::filesystem::path    workspace         = paths.workspace / workspace_name;
    const std::filesystem::path    prepare_dir       = prepare_path(paths, workspace_name);
    const std::vector<std::string> workspace_content = io::content_lines(workspace, 1, 1, error_message);
    const std::string              version_id        = workspace_content.empty() ? "" : workspace_content[0];
    std::vector<std::string>       ignore_raw        = io::content_lines(paths.ignore, 0, 0, error_message);
    std::vector<std::string>       ignore            = paths.resolve(ignore_raw, version_id, 1, 1);
    CHECKERROR(error_message);

    std::vector<std::string> input = paths.resolve(input_raw, version_id, 1, 1);
    CHECKERROR(error_message);
    if (input.empty())
        return;

    std::vector<std::string> current = io::content_lines(prepare_dir, 1, 1, error_message);

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

    std::unordered_map<std::filesystem::path, object::info> version =
        version::map_all_objects(paths, version_id, ignore_entries, error_message);
    std::erase_if(current, [&version, &paths, &error_message](std::string& entry) {
        if (entry.starts_with(PREFIX_DELETED))
            return false;
        std::string buffer = io::content(paths.root / entry, 0, 0, error_message);
        insert_pattern(buffer, TYPE_BLOB, nullptr);
        char id[65];
        sha256(buffer.c_str(), buffer.size(), id);
        const auto it = version.find(entry);
        if (it == version.end())
            return false;
        return id == it->second.id;
    });

    std::string buffer;
    for (std::string& path : current)
        buffer += path + "\n";

    insert_pattern(buffer, TYPE_PREPARE, nullptr);
    const Pending pending = {.type = PendingType::rename, .src = paths.get_tmp_file(), .dest = prepare_dir};
    io::file(pending.src, std::ios::binary | std::ios::trunc, buffer.data(), buffer.size(), 1, error_message);
    paths.move_pending(pending);

    if (prepared)
        *prepared = strvector_to_charpp(current);
}