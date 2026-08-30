#include <core.hpp>

bool workspace::exists(std::filesystem::path workspace_dir, std::string name, char** error_message) {
    try {
        std::filesystem::recursive_directory_iterator iterator(workspace_dir, std::filesystem::directory_options::skip_permission_denied);
        for(const std::filesystem::directory_entry& entry : iterator)
            if(entry.is_regular_file() && entry.path().filename() == name) return true;
    } catch(const std::filesystem::filesystem_error& error) {
        error_message_creator("Directory iteration failure", error_message);
    }
    return false;
}

bool workspace::exists(std::filesystem::path workspace_dir, std::string name, std::string& path, char** error_message) {
    try {
        std::filesystem::recursive_directory_iterator iterator(workspace_dir, std::filesystem::directory_options::skip_permission_denied);
        for(const std::filesystem::directory_entry& entry : iterator) {
            if(entry.is_regular_file() && entry.path().filename() == name) {
                path = entry.path();
                return true;
            }
        }
    } catch(const std::filesystem::filesystem_error& error) {
        error_message_creator("Directory iteration failure", error_message);
    }
    return false;
}