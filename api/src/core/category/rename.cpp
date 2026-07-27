#include <core.hpp>

LvcError category::rename(const std::filesystem::path& workspace_dir, const char* category_name, const char* new_name) {
    std::error_code ec;
    std::filesystem::rename(workspace_dir / category_name, workspace_dir / new_name, ec);
    return ec ? CATEGORY_FOLDER_RENAME : SUCCESS;
}