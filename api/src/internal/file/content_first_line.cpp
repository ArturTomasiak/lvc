#include "internal.hpp"
// TODO deflated case
std::string file::content_first_line(std::filesystem::path file_path, bool deflated) {
    std::string result;

    std::ifstream file(file_path, std::ios::binary);
    if (!file.is_open())
        return result;

    std::getline(file, result);
    if (!result.empty() && result.back() == '\r')
        result.pop_back();
    return result;
}