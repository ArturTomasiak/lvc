#include <core.hpp>

LvcError category::create(std::filesystem::path workspace_dir, std::string name) {
    if (category::exists(workspace_dir, name))
        return CATEGORY_EXISTS;
    if (!dir_create(workspace_dir / name))
        return CATEGORY_FOLDER_CREATE;
    return SUCCESS;
}