#include <core.hpp>

void category::change(Paths& paths, std::string workspace_name, const char* name, char** error_message) {
    PendingRename pending = {.tmp = paths.tmp / workspace_name, .dest = paths.workspace / workspace_name};
    std::string   buffer  = io::content(pending.dest, 1, 1, error_message);
    insert_pattern(buffer, TYPE_WORKSPACE, name);
    io::file(pending.tmp, std::ios::binary | std::ios::trunc, buffer.data(), buffer.size(), 1, error_message);
    paths.pending_rename.emplace_back(std::move(pending));
}