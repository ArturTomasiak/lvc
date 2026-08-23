#include <core.hpp>

void repository::create(std::filesystem::path lvc, char** error_message) {
    if (!io::dir(lvc, error_message)) {
        error_message_creator("Failed to create directory" + lvc.string(), error_message);
        return;
    }
    if (!io::dir(lvc / NAME_WORKSPACE, error_message)) {
        error_message_creator("Failed to create directory" + (lvc / NAME_WORKSPACE).string(), error_message);
        return;
    }
    if (!io::dir(lvc / NAME_OBJECT, error_message)) {
        error_message_creator("Failed to create directory" + (lvc / NAME_OBJECT).string(), error_message);
        return;
    }
    category::create(lvc, "inactive", error_message);
}