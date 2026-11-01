#include "core.hpp"

void operation::create(
    Paths& paths, const std::string& dest_workspace_name, const std::string& src_workspace_name, operation::type type,
    LvcConflictArray* conflicts, char** error_message) {
    const std::filesystem::path operation = paths.local / dest_workspace_name;
    if (type == operation::type::merge) {
        PendingRename pending = {.tmp = paths.tmp / filename_merge, .dest = operation / filename_merge};
        io::file(
            pending.dest, std::ios::binary | std::ios::trunc, src_workspace_name.data(), src_workspace_name.size(), 0,
            error_message);
        paths.pending_rename.emplace_back(std::move(pending));
    }
    if (conflicts->len != 0) {
        PendingRename pending = {.tmp = paths.tmp / filename_conflict, .dest = operation / filename_conflict};
        std::ofstream conflict(pending.dest, std::ios::binary | std::ios::trunc | std::ios::out);
        for (size_t i = 0; i < conflicts->len; i++) {
            conflict << conflicts->paths[i];
            conflict << '\n';
        }
        conflict.close();
        paths.pending_rename.emplace_back(std::move(pending));
    }
    ERRORCHECK(error_message);
    paths.pending_directory.push_back(std::move(operation));
}