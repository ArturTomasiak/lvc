#include <core.hpp>

std::string file::content(std::filesystem::path file_path, bool deflated) {
    std::ifstream file(file_path, std::ios::binary);
    std::string out;
    if (!file)
        return out;
    std::ostringstream buffer;
    buffer << file.rdbuf();
    if (!file.eof() && file.fail())
        return out;

    out = buffer.str();
    if (deflated) {
        uint64_t inflated_len;
        char*    inflated = object::inflate(out, inflated_len);
        out.assign(inflated, inflated_len);
    }

    return out;
}