#include <core.hpp>

void category::create(Paths& paths, std::string name, char** error_message) {
    std::filesystem::path category = paths.workspace / name;
    if (std::filesystem::is_directory(category)) {
        error_message_creator("Category already exists", error_message);
        return;
    }
    if (!io::dir(category, error_message)) {
        error_message_creator("Failed to create folder", error_message);
        return;
    }
}