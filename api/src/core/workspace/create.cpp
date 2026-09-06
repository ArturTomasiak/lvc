#include <core.hpp>

void workspace::create(
    Paths& paths, std::string category_name, std::string workspace_name, bool clone_working, char** error_message) {
    if (category_name == NAME_INACTIVE) {
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
    io::file(paths.workspace / category_name / workspace_name, std::ios::binary, error_message);
    if (!clone_working || *error_message)
        return;
    std::filesystem::path operation = paths.local / workspace_name;
    version::create_tmp(paths, operation, error_message);
}