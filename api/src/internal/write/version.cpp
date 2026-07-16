#include <internal.hpp>

LvcError write::version(std::filesystem::path lvc, const char* message) {
    std::filesystem::path working_dir = lvc.parent_path();
    std::filesystem::path branch      = lvc / NAME_WORKSPACE / get::file_content(lvc / NAME_CURRENT);
    std::vector<std::string> status   = get::status(lvc);
    return SUCCESS;
}