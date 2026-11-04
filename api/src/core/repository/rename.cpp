#include <core.hpp>

void repository::rename(Paths& paths, const char* name, char** error_message) {
    const std::filesystem::path tmp = paths.tmp_file();
    io::file(tmp, std::ios::binary | std::ios::trunc, name, charplen(name), 0, error_message);
    paths.add_pending({.type = PendingType::rename, .src = tmp, .dest = paths.name});
}