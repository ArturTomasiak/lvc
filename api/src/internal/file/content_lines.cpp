#include <internal.hpp>
// TODO deflated case
std::vector<std::string> file::content_lines(std::filesystem::path file_path, bool deflated) {
    std::vector<std::string> result;
    result.reserve(PREALLOCATE_SMALL);

    std::ifstream file(file_path, std::ios::binary);
    if (!file.is_open())
        return result;

    std::string line;

    while (std::getline(file, line)) {
        if (!line.empty() && line.back() == '\r')
            line.pop_back();
        result.push_back(line);
    }
    return result;
}