#include "core.hpp"

void operation::create(
    Paths& paths, const std::string& dest_workspace_name, const std::string& src_workspace_name, operation::type type,
    LvcConflictArray* conflicts, char** error_message) {
    const Pending operation = {.type = PendingType::create_directory, .dest = paths.local / dest_workspace_name};
    const Pending merge     = {
            .type = PendingType::rename, .src = paths.get_tmp_file(), .dest = operation.dest / filename_merge};
    const Pending conflict = {
        .type = PendingType::rename, .src = paths.get_tmp_file(), .dest = operation.dest / filename_conflict};
    paths.move_pending(operation);
    if (type == operation::type::merge) {
        io::file(
            merge.src, std::ios::binary | std::ios::trunc, src_workspace_name.data(), src_workspace_name.size(), 0,
            error_message);
        paths.move_pending(merge);
    }
    if (conflicts->len != 0) {
        std::ofstream conflict_stream(conflict.src, std::ios::binary | std::ios::trunc | std::ios::out);
        for (size_t i = 0; i < conflicts->len; i++) {
            conflict_stream << conflicts->paths[i];
            conflict_stream << '\n';
        }
        conflict_stream.close();
        paths.move_pending(conflict);
    }
}