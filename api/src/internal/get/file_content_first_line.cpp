#include "internal.hpp"

std::string get::file_content_first_line(std::filesystem::path file_path) {
    std::string result;

    std::ifstream file(file_path);
    if (!file.is_open())
        return result;

    std::getline(file, result);
    return result;
}