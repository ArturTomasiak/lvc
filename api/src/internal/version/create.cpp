#include <internal.hpp>

LvcError version::create(std::filesystem::path lvc, const char* message) {
    if (!message)
        return VERSION_NO_MESSAGE;
    std::filesystem::path working_dir = lvc.parent_path();
    std::filesystem::path branch      = lvc / NAME_WORKSPACE / file::content(lvc / NAME_CURRENT, 0);
    std::vector<std::string> status   = version::status(lvc);
    // TODO
    std::error_code error;
    bool deleted = std::filesystem::remove(lvc / NAME_STATUS, error);

    return SUCCESS;
}