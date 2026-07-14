#include <internal.hpp>

bool exists::workspace(std::filesystem::path lvc, std::string name) {
    lvc /= "workspace";
    for (const std::filesystem::directory_entry& entry : std::filesystem::recursive_directory_iterator(lvc))
        if (entry.is_regular_file() && entry.path().filename() == name)
            return true;
    return false;
}

bool exists::workspace(std::filesystem::path lvc, std::string name, std::string& path) {
    lvc /= "workspace";
    for (const std::filesystem::directory_entry& entry : std::filesystem::recursive_directory_iterator(lvc)) {
        if (entry.is_regular_file() && entry.path().filename() == name) {
            path = entry.path();
            return true;
        }
    }
    return false;
}