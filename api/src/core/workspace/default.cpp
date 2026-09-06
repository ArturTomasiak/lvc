#include "core.hpp"

void workspace::_default(Paths& paths, const char* workspace_name, char** error_message) {
    if (!workspace::exists(paths, workspace_name, error_message)) {
        std::string workspace_str = workspace_name;
        error_message_creator("Workspace " + workspace_str + " doesn't exist", error_message);
    }
    if (*error_message)
        return;

    if (workspace::is_inactive(paths, workspace_name)) {
        error_message_creator("Cannot make inactive workspace default", error_message);
        return;
    }

    io::file(paths.lvc / NAME_DEFAULT, std::ios::binary, workspace_name, charplen(workspace_name), 0, error_message);
}