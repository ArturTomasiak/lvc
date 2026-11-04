#include <core.hpp>

void category::change(Paths& paths, std::string workspace_name, const char* name, char** error_message) {
    const std::string workspace = paths.workspace / workspace_name;
    std::string       buffer    = io::content(workspace, 1, 1, error_message);
    insert_pattern(buffer, TYPE_WORKSPACE, name);
    const Pending pending = {.type = PendingType::rename, .src = paths.tmp_file(), .dest = workspace};
    io::file(pending.src, std::ios::binary | std::ios::trunc, buffer.data(), buffer.size(), 1, error_message);
    paths.move_pending(pending);
}