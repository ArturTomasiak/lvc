#include <internal.hpp>

LvcError repository::create(std::filesystem::path lvc) {
    if (!create::dir(lvc))
        return LVC_FOLDER_CREATE;
    if (!create::dir(lvc / NAME_WORKSPACE))
        return WORKSPACE_FOLDER_CREATE;
    if (!create::dir(lvc / NAME_OBJECT))
        return OBJECT_FOLDER_CREATE;
    RETURN_ERR(category::create(lvc, "inactive"));
    return SUCCESS;
}