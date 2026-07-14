#include <internal.hpp>

std::vector<std::string> get::status_all(std::filesystem::path lvc) {
    std::vector<std::string> status = get::file_lines(lvc / "status");
    return status;
}