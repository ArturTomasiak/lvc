#include "core.hpp"

void operation::create(
    Paths& paths, const std::string& dest_workspace_name, const std::string& src_workspace_name, operation::type type,
    LvcConflictArray* conflicts, char** error_message) {
    const std::filesystem::path operation = paths.local / dest_workspace_name;
    if (type == operation::type::merge) {
        const std::filesystem::path merge_path = operation / filename_merge;
        const std::filesystem::path merge_tmp  = paths.add_pending_rename(merge_path);
        io::file(
            merge_tmp, std::ios::binary | std::ios::trunc, src_workspace_name.data(), src_workspace_name.size(), 0,
            error_message);
    }
    if (conflicts->len != 0) {
        const std::filesystem::path conflict_path = operation / filename_conflict;
        const std::filesystem::path conflict_tmp  = paths.add_pending_rename(conflict_path);
        std::ofstream               conflict(conflict_tmp, std::ios::binary | std::ios::trunc | std::ios::out);
        for (size_t i = 0; i < conflicts->len; i++) {
            conflict << conflicts->paths[i];
            conflict << '\n';
        }
        conflict.close();
    }
    ERRORCHECK(error_message);
    paths.pending_directory.push_back(std::move(operation));
}