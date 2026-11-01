#include <core.hpp>

void repository::rename(Paths& paths, const char* name, char** error_message) {
    const std::filesystem::path dest = paths.lvc / filename_name;
    const std::filesystem::path tmp  = paths.add_pending_rename(dest);
    io::file(tmp, std::ios::binary | std::ios::trunc, name, charplen(name), 0, error_message);
}