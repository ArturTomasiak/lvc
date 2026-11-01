#include <core.hpp>

void repository::rename(Paths& paths, const char* name, char** error_message) {
    PendingRename pending = {.tmp = paths.tmp / filename_name, .dest = paths.lvc / filename_name};
    io::file(pending.tmp, std::ios::binary | std::ios::trunc, name, charplen(name), 0, error_message);
    paths.pending_rename.emplace_back(std::move(pending));
}