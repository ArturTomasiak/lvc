#include <core.hpp>

void category::create(std::filesystem::path workspace_dir, std::string name, char** error_message) {
    if (category::exists(workspace_dir, name)) {
        error_message_creator("Category already exists", error_message);
        return;
    }
    if (!io::dir(workspace_dir / name, error_message)) {
        error_message_creator("Failed to create folder", error_message);
        return;
    }
}