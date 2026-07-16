#include <internal.hpp>

LvcError write::version(std::filesystem::path lvc, const char* message) {
    std::filesystem::path working_dir = lvc.parent_path();
    std::filesystem::path branch      = lvc / "workspace" / get::file_content(lvc / "current");
    std::vector<std::string> status   = get::status(lvc);
    return SUCCESS;
}