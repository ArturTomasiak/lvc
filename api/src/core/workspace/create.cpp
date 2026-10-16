#include <core.hpp>

void workspace::create(
    Paths& paths, std::string category_name, std::string workspace_name, bool clean, char** error_message) {
    if (category_name == workspacename_inactive) {
        error_message_creator("Creating a workspace in inactive category is forbidden", error_message);
        return;
    }
    if (!category::exists(paths, category_name)) {
        error_message_creator("Category" + category_name + "does not exist", error_message);
        return;
    }
    if (workspace::exists(paths, workspace_name, error_message)) {
        error_message_creator("Workspace already exists", error_message);
        return;
    }
    if (*error_message)
        return;
    if (clean) {
        io::file(paths.workspace / category_name / workspace_name, std::ios::binary | std::ios::trunc, error_message);
        return;
    }
    std::string current = io::content(paths.current, 0, error_message);
    std::string buffer  = io::content(workspace_path(paths.workspace, current), 0, error_message);
    io::file(
        paths.workspace / category_name / workspace_name, std::ios::binary | std::ios::trunc, buffer.data(),
        buffer.size(), 0, error_message);
}