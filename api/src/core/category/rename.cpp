#include <core.hpp>

void category::rename(const std::filesystem::path& workspace_dir, const char* category_name, const char* new_name, char** error_message) {
    std::error_code ec;
    std::filesystem::rename(workspace_dir / category_name, workspace_dir / new_name, ec);
    if (ec) {
        error_message_creator_path("Failed to rename category", category_name, error_message);
    }
}