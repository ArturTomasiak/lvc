#include <core.hpp>

bool category::exists(std::filesystem::path workspace_dir, std::string name) {
    workspace_dir /= name;
    return std::filesystem::is_directory(workspace_dir);
}