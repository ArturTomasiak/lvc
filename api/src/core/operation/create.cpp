#include "core.hpp"

void operation::create(
    Paths& paths, const std::string& dest_workspace_name, const std::string& src_workspace_name, operation::type type,
    LvcConflictArray* conflicts, char** error_message) {
    const std::filesystem::path operation = paths.local / dest_workspace_name;
    io::dir(operation, error_message);
    if (type == operation::type::merge)
        io::file(
            operation / filename_merge, std::ios::binary | std::ios::trunc, src_workspace_name.data(),
            src_workspace_name.size(), 0, error_message);
    if (conflicts->len == 0)
        return;
    std::ofstream conflict(operation / filename_conflict, std::ios::binary | std::ios::trunc | std::ios::out);
    for (size_t i = 0; i < conflicts->len; i++) {
        conflict << conflicts->paths[i];
        conflict << '\n';
    }
    conflict.close();
}