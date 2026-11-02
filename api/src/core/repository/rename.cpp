#include <core.hpp>

void repository::rename(Paths& paths, const char* name, char** error_message) {
    io::file(paths.lvc / filename_name, std::ios::binary | std::ios::trunc, name, charplen(name), 0, error_message);
}