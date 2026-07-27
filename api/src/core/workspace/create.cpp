#include <core.hpp>

LvcError workspace::create(std::filesystem::path workspace_dir, std::string category_name, std::string workspace_name) {
    if (category_name == NAME_INACTIVE)
        return CREATE_WORKSPACE_INACTIVE;
    if (!category::exists(workspace_dir, category_name))
        return CATEGORY_NOT_EXISTS;
    if (workspace::exists(workspace_dir, workspace_name))
        return WORKSPACE_EXISTS;
    if (!file_create(workspace_dir / category_name / workspace_name, std::ios::binary))
        return WORKSPACE_FILE_CREATE;
    return SUCCESS;
}