#include <core.hpp>

void version::prepare_reset(std::filesystem::path lvc, char** error_message) {
    const std::string           workspace_name = io::content(lvc / NAME_CURRENT, 0, error_message);
    const std::filesystem::path workspace_dir  = lvc / NAME_WORKSPACE;
    const std::filesystem::path prepare_dir    = workspace_dir / NAME_LOCAL / workspace_name / NAME_PREPARE;
    std::error_code             error;
    bool                        deleted = std::filesystem::remove(prepare_dir, error);
    if(!deleted || error) error_message_creator("Could not reset prepare", error_message);
}

void version::prepare(std::filesystem::path lvc, std::vector<std::string> input_raw, char*** prepared, char** error_message) {
    if(input_raw.empty()) {
        error_message_creator("No input recieved", error_message);
        return;
    }

    const std::string           workspace_name = io::content(lvc / NAME_CURRENT, 0, error_message);
    const std::filesystem::path workspace_dir  = lvc / NAME_WORKSPACE;
    const std::filesystem::path workspace      = workspace_path(workspace_dir, workspace_name);
    const std::filesystem::path object_dir     = lvc / NAME_OBJECT;
    const std::filesystem::path working_dir    = lvc.parent_path();
    const std::filesystem::path prepare_dir    = workspace_dir / NAME_LOCAL / workspace_name / NAME_PREPARE;
    const std::string           version_id     = io::content_first_line(workspace, error_message);

    std::vector<std::string> ignore_raw = io::content_lines(working_dir / NAME_IGNORE, 0, error_message);
    std::vector<std::string> ignore     = path_from_input(working_dir, ignore_raw, version_id, 1, error_message);
    if(*error_message) return;

    std::vector<std::string> input = path_from_input(working_dir, input_raw, version_id, 1, error_message);
    if(*error_message) return;
    if(input.empty()) return;

    std::vector<std::string> current = io::content_lines(prepare_dir, 0, error_message);

    std::unordered_set<std::string_view> current_entries;
    current_entries.reserve(current.size());
    for(const std::string& path : current) current_entries.insert(path);

    for(const std::string& path : input)
        if(!current_entries.contains(path)) current.push_back(path);

    std::unordered_set<std::string_view> ignore_entries;
    ignore_entries.reserve(ignore.size());
    for(const std::string& entry : ignore) ignore_entries.insert(entry);

    std::erase_if(current, [&ignore_entries](const std::string& entry) { return ignore_entries.contains(entry); });

    std::string buffer;

    for(std::string& path : current) {
        const std::filesystem::path object = object_dir / path;
        buffer += path + "\n";
    }

    io::file(prepare_dir, std::ios::binary, buffer.data(), buffer.size(), 0, error_message);

    if(prepared) *prepared = strvector_to_charpp(current);
}