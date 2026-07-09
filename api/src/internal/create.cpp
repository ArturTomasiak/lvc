#include <internal.hpp>

static bool create_dir(std::filesystem::path lvc) {
    std::error_code error;
    if (!std::filesystem::create_directory(lvc, error) || error) {
        #ifdef TEST_PRINTS
        if (error)
            std::cout << error.message() << "\n";
        #endif
        return 0;
    }
    return 1;
}

LvcError create::lvc(std::filesystem::path lvc) {
    if (!create_dir(lvc))
        return LVC_FOLDER_CREATE;
    if (!create_dir(lvc / "workspace"))
        return WORKSPACE_FOLDER_CREATE;
    if (!create_dir(lvc / "object"))
        return OBJECT_FOLDER_CREATE;
    return SUCCESS;
}

LvcError create::category(std::filesystem::path lvc, std::string name) {
    if (exists::category(lvc, name))
        return CATEGORY_EXISTS;
    if (!create_dir(lvc / "workspace" / name))
        return CATEGORY_FOLDER_CREATE;
    return SUCCESS;
}

LvcError create::workspace(std::filesystem::path lvc, std::string category_name, std::string workspace_name) {
    if (!exists::category(lvc, category_name))
        return CATEGORY_NOT_EXISTS;
    if (exists::workspace(lvc, workspace_name, 0))
        return WORKSPACE_EXISTS;
    if (!write::file(lvc / "workspace" / category_name / workspace_name, std::ios::binary, 0))
        return WORKSPACE_FILE_CREATE;
    return SUCCESS;
}

LvcError create::storage(std::filesystem::path lvc, StorageBehaviour option) {
    return SUCCESS;
}