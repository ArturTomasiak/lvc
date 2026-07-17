#include <internal.hpp>

bool workspace::exists(std::filesystem::path workspace_dir, std::string name) {
    for (const std::filesystem::directory_entry& entry : std::filesystem::recursive_directory_iterator(workspace_dir))
        if (entry.is_regular_file() && entry.path().filename() == name)
            return true;
    return false;
}

bool workspace::exists(std::filesystem::path workspace_dir, std::string name, std::string& path) {
    for (const std::filesystem::directory_entry& entry : std::filesystem::recursive_directory_iterator(workspace_dir)) {
        if (entry.is_regular_file() && entry.path().filename() == name) {
            path = entry.path();
            return true;
        }
    }
    return false;
}