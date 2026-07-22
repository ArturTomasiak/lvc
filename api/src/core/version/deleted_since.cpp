#include <core.hpp>

std::vector<std::filesystem::path> version::deleted_since(std::filesystem::path lvc, std::string id) {
    const std::filesystem::path object = lvc / NAME_OBJECT;
    const std::filesystem::path working_directory = lvc.parent_path();
    std::vector<std::filesystem::path> out;
    return out;
}