#include <internal.hpp>

static bool create_dir(std::filesystem::path lvc_path) {
    std::error_code error;
    if (!std::filesystem::create_directory(lvc_path, error) || error)
        return 0;
    return 1;
}

static bool create_file(std::filesystem::path lvc_path) {
    return 1;
}

LvcError create::lvc(std::filesystem::path lvc_path) {
    if (!create_dir(lvc_path))
        return LVC_FOLDER_CREATE;
    if (!create_dir(lvc_path / "workspace"))
        return WORKSPACE_FOLDER_CREATE;
    if (!create_dir(lvc_path / "object"))
        return OBJECT_FOLDER_CREATE;
    return SUCCESS;
}

LvcError create::category(std::filesystem::path lvc_path, std::string name) {
    if (exists::category(lvc_path, name))
        return CATEGORY_EXISTS;
    if (!create_dir(lvc_path / name))
        return CATEGORY_FOLDER_CREATE;
    return SUCCESS;
}

LvcError create::workspace(std::filesystem::path lvc_path, std::string name) {
    if (!exists::category(lvc_path, name))
        return CATEGORY_NOT_EXISTS;
    if (exists::workspace(lvc_path, name))
        return WORKSPACE_EXISTS;
    if (!create_dir(lvc_path / name))
        return WORKSPACE_FILE_CREATE;
    return SUCCESS;
}

LvcError create::storage(std::filesystem::path lvc_path, StorageBehaviour option) {
    return SUCCESS;
}