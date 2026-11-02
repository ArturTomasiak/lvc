#include "core.hpp"

void workspace::_default(Paths& paths, const char* workspace_name, char** error_message) {
    if (!workspace::exists(paths, workspace_name, error_message)) {
        std::string workspace_str = workspace_name;
        error_message_creator("Workspace " + workspace_str + " doesn't exist", error_message);
    }
    ERRORCHECK(error_message);
    std::string           name = workspace_name;
    std::filesystem::path path;
    io::file(
        paths.lvc / filename_default, std::ios::binary | std::ios::trunc, workspace_name, charplen(workspace_name), 0,
        error_message);
}