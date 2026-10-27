#include <core.hpp>

void category::change(std::filesystem::path workspace, const char* name, char** error_message) {
    std::string buffer = io::content(workspace, 1, 1, error_message);
    insert_pattern(buffer, TYPE_WORKSPACE, name);
    io::file(workspace, std::ios::binary | std::ios::trunc, buffer.data(), buffer.size(), 1, error_message);
}