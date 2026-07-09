#include <internal.hpp>

bool exists::category(std::filesystem::path lvc, std::string name) {
    lvc /= "workspace";
    lvc /= name;
    return std::filesystem::exists(lvc) && std::filesystem::is_directory(lvc);
}

bool exists::workspace(std::filesystem::path lvc, std::string name, std::string* path) {
    lvc /= "workspace";
    for (const std::filesystem::directory_entry& entry : std::filesystem::recursive_directory_iterator(lvc)) {
        if (entry.is_regular_file() && entry.path().filename() == name) {
            if (path) {
                *path = entry.path();
            }
            return true;
        }
    }
    return false;
}