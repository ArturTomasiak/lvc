#include <core.hpp>

LvcError workspace::create(std::filesystem::path workspace_dir, std::string category_name, std::string workspace_name) {
    if (category_name == NAME_INACTIVE)
        return CREATE_WORKSPACE_INACTIVE;
    if (!category::exists(workspace_dir, category_name))
        return CATEGORY_NOT_EXISTS;
    LvcError err;
    if (workspace::exists(workspace_dir, workspace_name, err))
        return WORKSPACE_EXISTS;
    if (err) return err;
    RETURN_ERR(io::file(workspace_dir / category_name / workspace_name, std::ios::binary));
    return SUCCESS;
}