#include <core.hpp>

void workspace::create(
    Paths& paths, std::string workspace_name, std::string copy_workspace_name, char** error_message) {
    PendingRename pending = {.tmp = paths.tmp / workspace_name, .dest = paths.workspace / workspace_name};
    if (std::filesystem::is_regular_file(pending.dest)) {
        error_message_creator("Workspace already exists", error_message);
        return;
    }
    std::string buffer;
    if (!copy_workspace_name.empty())
        buffer = io::content(paths.workspace / copy_workspace_name, 1, 1, error_message);
    insert_pattern(buffer, TYPE_WORKSPACE, nullptr);
    io::file(pending.tmp, std::ios::binary | std::ios::trunc, buffer.data(), buffer.size(), 1, error_message);
    paths.pending_rename.emplace_back(std::move(pending));
}