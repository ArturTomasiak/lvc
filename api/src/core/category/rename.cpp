#include <core.hpp>

void category::rename(Paths& paths, const char* category_name, const char* new_name, char** error_message) {
    std::error_code ec;
    std::filesystem::rename(paths.workspace / category_name, paths.workspace / new_name, ec);
    if (ec) {
        error_message_creator_path("Failed to rename category", category_name, error_message);
    }
}