#include <core.hpp>

void repository::rename(std::filesystem::path lvc, const char* name, char** error_message) {
    io::file(lvc / NAME_NAME, std::ios::binary, name, charplen(name), 0, error_message);
}