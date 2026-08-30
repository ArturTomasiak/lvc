#include <core.hpp>

void workspace::create(std::filesystem::path workspace_dir, std::string category_name, std::string workspace_name, char** error_message) {
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
}