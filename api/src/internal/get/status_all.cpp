#include <internal.hpp>

std::vector<std::string> get::status_all(std::filesystem::path lvc) {
    return get::file_content_lines(lvc / NAME_STATUS);
}