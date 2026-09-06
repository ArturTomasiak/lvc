#include "core.hpp"

void operation::create_folder(const std::filesystem::path& operation, operation::type type_enum, char** error_message) {
    io::dir(operation, error_message);
    std::string type;
    switch (type_enum) {
        case operation::type::sync:
            type = operation::filename_sync;
            break;
        case operation::type::unite:
            type = operation::filename_unite;
            break;
        case operation::type::insert:
            type = operation::filename_insert;
            break;
        default:
            error_message_creator("APIERROR OperationType doesn't exist", error_message);
            break;
    }
    io::file(operation / type, std::ios::binary, error_message);
}