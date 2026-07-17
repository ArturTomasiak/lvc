#include <internal.hpp>
// TODO deflated case
std::string get::file_content(std::filesystem::path file_path, bool deflated) {
    std::ifstream file(file_path, std::ios::binary);
    std::string empty;
    if (!file)
        return empty;
    std::ostringstream buffer;
    buffer << file.rdbuf();
    if (!file.eof() && file.fail())
        return empty;
    return buffer.str();
}