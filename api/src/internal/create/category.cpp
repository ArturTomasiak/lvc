#include <internal.hpp>

LvcError create::category(std::filesystem::path lvc, std::string name) {
    if (exists::category(lvc, name))
        return CATEGORY_EXISTS;
    if (!create::dir(lvc / "workspace" / name))
        return CATEGORY_FOLDER_CREATE;
    return SUCCESS;
}