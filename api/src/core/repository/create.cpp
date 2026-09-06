#include <core.hpp>

void repository::create(Paths& paths, char** error_message) {
    if (!io::dir(paths.lvc, error_message)) {
        error_message_creator("Failed to create directory" + paths.lvc.string(), error_message);
        return;
    }
    if (!io::dir(paths.workspace, error_message)) {
        error_message_creator("Failed to create directory" + (paths.workspace).string(), error_message);
        return;
    }
    if (!io::dir(paths.object, error_message)) {
        error_message_creator("Failed to create directory" + paths.object.string(), error_message);
        return;
    }
    category::create(paths, "inactive", error_message);
}