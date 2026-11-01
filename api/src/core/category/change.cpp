#include <core.hpp>

void category::change(Paths& paths, std::string workspace_name, const char* name, char** error_message) {
    const std::filesystem::path workspace_path = paths.workspace / workspace_name;
    const std::filesystem::path workspace_tmp  = paths.add_pending_rename(workspace_path);
    std::string                 buffer         = io::content(workspace_path, 1, 1, error_message);
    insert_pattern(buffer, TYPE_WORKSPACE, name);
    io::file(workspace_tmp, std::ios::binary | std::ios::trunc, buffer.data(), buffer.size(), 1, error_message);
}