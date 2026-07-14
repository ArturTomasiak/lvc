#include <internal.hpp>

LvcError create::lvc(std::filesystem::path lvc) {
    if (!create::dir(lvc))
        return LVC_FOLDER_CREATE;
    if (!create::dir(lvc / "workspace"))
        return WORKSPACE_FOLDER_CREATE;
    if (!create::dir(lvc / "object"))
        return OBJECT_FOLDER_CREATE;
    RETURN_ERR(create::category(lvc, "inactive"));
    return SUCCESS;
}