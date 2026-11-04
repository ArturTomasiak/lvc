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
    const Pending pending = {.type = PendingType::rename, .src = paths.tmp_file(), .dest = workspace};
    io::file(pending.src, std::ios::binary | std::ios::trunc, buffer.data(), buffer.size(), 1, error_message);
    paths.move_pending(pending);
}