#include "core.hpp"

void workspace::_default(Paths& paths, const char* workspace_name, char** error_message) {
    if (!std::filesystem::is_regular_file(paths.workspace / workspace_name)) {
        error_message_creator("Workspace " + std::string(workspace_name) + " doesn't exist", error_message);
    }
    CHECKERROR(error_message);
    io::file(
        paths.lvc / filename_default, std::ios::binary | std::ios::trunc, workspace_name, charplen(workspace_name), 0,
        error_message);
}