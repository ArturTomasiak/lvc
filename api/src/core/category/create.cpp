#include <core.hpp>

void category::create(std::filesystem::path lvc, std::string name, char** error_message) {
    std::filesystem::path category = lvc / NAME_WORKSPACE / name;
    if(std::filesystem::is_directory(category)) {
        error_message_creator("Category already exists", error_message);
        return;
    }
    if(!io::dir(category, error_message)) {
        error_message_creator("Failed to create folder", error_message);
        return;
    }
}