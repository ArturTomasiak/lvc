#include <internal.hpp>

LvcError create::category(std::filesystem::path workspace_dir, std::string name) {
    if (exists::category(workspace_dir, name))
        return CATEGORY_EXISTS;
    if (!create::dir(workspace_dir / name))
        return CATEGORY_FOLDER_CREATE;
    return SUCCESS;
}