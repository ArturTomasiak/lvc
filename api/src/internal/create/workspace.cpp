#include <internal.hpp>

LvcError create::workspace(std::filesystem::path lvc, std::string category_name, std::string workspace_name) {
    if (!exists::category(lvc, category_name))
        return CATEGORY_NOT_EXISTS;
    if (exists::workspace(lvc, workspace_name))
        return WORKSPACE_EXISTS;
    if (!write::file(lvc / "workspace" / category_name / workspace_name, std::ios::binary))
        return WORKSPACE_FILE_CREATE;
    return SUCCESS;
}