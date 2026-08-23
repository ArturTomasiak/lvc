#include <core.hpp>

void workspace::_goto(std::filesystem::path lvc, const char* workspace_name, char** error_message) {
    std::filesystem::path workspace_dir = lvc / NAME_WORKSPACE;
    
    if (!workspace::exists(workspace_dir, workspace_name, error_message)) {
        std::string workspace_str = workspace_name;
        error_message_creator("Workspace " + workspace_str + " doesn't exist", error_message);
    }
    if (error_message) return;

    io::file(lvc / NAME_CURRENT, std::ios::binary, workspace_name, charplen(workspace_name), 0, error_message);
}

void workspace::_default(std::filesystem::path lvc, const char* workspace_name, char** error_message) {
    std::filesystem::path workspace_dir = lvc / NAME_WORKSPACE;

    if (!workspace::exists(workspace_dir, workspace_name, error_message)) {
        std::string workspace_str = workspace_name;
        error_message_creator("Workspace " + workspace_str + " doesn't exist", error_message);
    }
    if (error_message) return;
    
    if (workspace::is_inactive(workspace_dir, workspace_name)) {
        error_message_creator("Cannot make inactive workspace default", error_message);
        return;
    }
        
    io::file(lvc / NAME_DEFAULT, std::ios::binary, workspace_name, charplen(workspace_name), 0, error_message);
}