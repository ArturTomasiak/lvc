#include <internal.hpp>

bool exists::category(std::filesystem::path lvc_path, std::string name) {
    std::filesystem::path path = lvc_path / "workspace" / name;
    return std::filesystem::exists(path) && std::filesystem::is_directory(path);
}

bool exists::workspace(std::filesystem::path lvc_path, std::string name) {
    std::filesystem::path path = lvc_path / "workspace";
    for (const auto& entry : std::filesystem::recursive_directory_iterator(path))
        if (entry.is_regular_file() && entry.path().filename() == name)
            return true;
    return false;
}