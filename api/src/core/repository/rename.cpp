#include <core.hpp>

void repository::rename(Paths& paths, const char* name, char** error_message) {
    const Pending pending = {.type = PendingType::rename, .src = paths.get_tmp_file(), .dest = paths.name};
    io::file(pending.src, std::ios::binary | std::ios::trunc, name, charplen(name), 0, error_message);
    paths.move_pending(pending);
}