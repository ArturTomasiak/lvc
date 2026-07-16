#include <internal.hpp>

std::string file_content(std::filesystem::path file_path) {
    std::ifstream file(file_path);
    std::string empty;
    if (!file)
        return empty;
    std::ostringstream buffer;
    buffer << file.rdbuf();
    if (!file.eof() && file.fail())
        return empty;
    return buffer.str();
}