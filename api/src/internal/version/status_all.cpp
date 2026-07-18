#include <internal.hpp>

std::vector<std::string> version::status_all(std::filesystem::path lvc) {
    return file::content_lines(lvc / NAME_STATUS, 0);
}