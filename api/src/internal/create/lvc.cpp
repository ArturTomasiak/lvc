#include <internal.hpp>

LvcError create::lvc(std::filesystem::path lvc) {
    if (!create::dir(lvc))
        return LVC_FOLDER_CREATE;
    if (!create::dir(lvc / NAME_WORKSPACE))
        return WORKSPACE_FOLDER_CREATE;
    if (!create::dir(lvc / NAME_OBJECT))
        return OBJECT_FOLDER_CREATE;
    RETURN_ERR(create::category(lvc, "inactive"));
    return SUCCESS;
}