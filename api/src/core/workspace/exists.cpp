#include <core.hpp>

bool workspace::exists(std::filesystem::path workspace_dir, std::string name) {
    std::filesystem::recursive_directory_iterator iterator(workspace_dir, std::filesystem::directory_options::skip_permission_denied);
    for (const std::filesystem::directory_entry& entry : iterator)
        if (entry.is_regular_file() && entry.path().filename() == name)
            return true;
    return false;
}

bool workspace::exists(std::filesystem::path workspace_dir, std::string name, std::string& path) {
    std::filesystem::recursive_directory_iterator iterator(workspace_dir, std::filesystem::directory_options::skip_permission_denied);
    for (const std::filesystem::directory_entry& entry : iterator) {
        if (entry.is_regular_file() && entry.path().filename() == name) {
            path = entry.path();
            return true;
        }
    }
    return false;
}