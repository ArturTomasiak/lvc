#include <core.hpp>

void workspace::create(Paths& paths, std::string workspace_name, bool clean, char** error_message) {
    std::filesystem::path workspace = paths.workspace / workspace_name;
    if (std::filesystem::is_regular_file(workspace)) {
        error_message_creator("Workspace already exists", error_message);
        return;
    }
    const std::string current = io::content(paths.current, 0, error_message);
    std::string       buffer;
    if (!clean)
        buffer = io::content(paths.workspace / current, 0, error_message);
    insert_pattern(buffer, TYPE_WORKSPACE, nullptr);
    io::file(workspace, std::ios::binary | std::ios::trunc, buffer.data(), buffer.size(), 1, error_message);
}