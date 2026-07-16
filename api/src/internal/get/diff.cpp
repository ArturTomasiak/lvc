#include <internal.hpp>

std::vector<std::string> get::diff(std::filesystem::path lvc) {
    std::filesystem::path branch = lvc / "workspace" / get::file_content(lvc / "current");
    std::vector<std::string> result;
    result.reserve(PREALLOCATE);
    // TODO
    return result;
}


