#include <core.hpp>

LvcError version::create(std::filesystem::path lvc, const char* message) {
    if (!message)
        return VERSION_NO_MESSAGE;
    std::filesystem::path working_dir = lvc.parent_path();
    std::string workspace_name        = content(lvc / NAME_CURRENT, 0);

    std::filesystem::path workspace = workspace_path(lvc / NAME_WORKSPACE, workspace_name);
    std::vector<std::string> status = version::status(lvc);
    // TODO
    std::error_code error;
    bool deleted = std::filesystem::remove(lvc / NAME_STATUS, error);

    return SUCCESS;
}