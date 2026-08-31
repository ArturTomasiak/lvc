#include <core.hpp>

void workspace::create(std::filesystem::path lvc, std::string category_name, std::string workspace_name, bool clone_working, char** error_message) {
    std::filesystem::path workspace_dir = lvc / NAME_WORKSPACE;
    if(category_name == NAME_INACTIVE) {
        error_message_creator("Creating a workspace in inactive category is forbidden", error_message);
        return;
    }
    if(!category::exists(workspace_dir, category_name)) {
        error_message_creator("Category" + category_name + "does not exist", error_message);
        return;
    }
    if(workspace::exists(workspace_dir, workspace_name, error_message)) {
        error_message_creator("Workspace already exists", error_message);
        return;
    }
    if(*error_message) return;
    io::file(workspace_dir / category_name / workspace_name, std::ios::binary, error_message);
    if (!clone_working || *error_message) return;
    std::filesystem::path operation = workspace_dir / NAME_LOCAL / workspace_name;
    version::create_tmp(lvc, operation, error_message);
}