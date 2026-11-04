#include <core.hpp>

void workspace::create(
    Paths& paths, std::string workspace_name, std::string copy_workspace_name, char** error_message) {
    const std::filesystem::path workspace = paths.workspace / workspace_name;
    if (std::filesystem::is_regular_file(workspace)) {
        error_message_creator("Workspace already exists", error_message);
        return;
    }
    std::string buffer;
    if (!copy_workspace_name.empty())
        buffer = io::content(paths.workspace / copy_workspace_name, 1, 1, error_message);
    insert_pattern(buffer, TYPE_WORKSPACE, nullptr);
    const std::filesystem::path tmp = paths.tmp_file();
    io::file(tmp, std::ios::binary | std::ios::trunc, buffer.data(), buffer.size(), 1, error_message);
    paths.add_pending({.type = PendingType::rename, .src = tmp, .dest = workspace});
}