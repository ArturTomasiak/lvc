#include <core.hpp>

void repository::rename(Paths& paths, const char* name, char** error_message) {
    io::file(paths.lvc / NAME_NAME, std::ios::binary, name, charplen(name), 0, error_message);
}