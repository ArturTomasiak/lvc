#include "core.hpp"

void workspace::_default(Paths& paths, const char* workspace_name, char** error_message) {
    if (!std::filesystem::is_regular_file(paths.workspace / workspace_name)) {
        error_message_creator("Workspace " + std::string(workspace_name) + " doesn't exist", error_message);
        return;
    }
    const Pending pending = {
        .type = PendingType::rename, .src = paths.get_tmp_file(), .dest = paths.lvc / filename_default};
    io::file(
        pending.src, std::ios::binary | std::ios::trunc, workspace_name, charplen(workspace_name), 0, error_message);
    paths.move_pending(pending);
}