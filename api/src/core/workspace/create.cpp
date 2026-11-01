#include <core.hpp>

void workspace::create(
    Paths& paths, std::string workspace_name, std::string copy_workspace_name, char** error_message) {
    const std::filesystem::path workspace_path = paths.workspace / workspace_name;
    const std::filesystem::path workspace_tmp  = paths.add_pending_rename(workspace_path);
    if (std::filesystem::is_regular_file(workspace_path)) {
        error_message_creator("Workspace already exists", error_message);
        return;
    }
    std::string buffer;
    if (!copy_workspace_name.empty())
        buffer = io::content(paths.workspace / copy_workspace_name, 1, 1, error_message);
    insert_pattern(buffer, TYPE_WORKSPACE, nullptr);
    io::file(workspace_tmp, std::ios::binary | std::ios::trunc, buffer.data(), buffer.size(), 1, error_message);
}