#include <core.hpp>

void category::change(std::filesystem::path workspace, const char* name, char** error_message) {
    std::string buffer = io::content(workspace, 1, error_message);
    size_t      pos    = buffer.find('\n');
    buffer.erase(0, pos + 1);
    insert_pattern(buffer, TYPE_WORKSPACE, name);
    io::file(workspace, std::ios::binary | std::ios::trunc, buffer.data(), buffer.size(), 1, error_message);
}