#include <internal.hpp>

LvcError create::workspace(std::filesystem::path workspace_dir, std::string category_name, std::string workspace_name) {
    if (!exists::category(workspace_dir, category_name))
        return CATEGORY_NOT_EXISTS;
    if (exists::workspace(workspace_dir, workspace_name))
        return WORKSPACE_EXISTS;
    if (!write::file(workspace_dir / category_name / workspace_name, std::ios::binary))
        return WORKSPACE_FILE_CREATE;
    return SUCCESS;
}