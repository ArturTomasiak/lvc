#include "core.hpp"

void operation::create(
    Paths& paths, const std::string& workspace_name, operation::type type_enum, LvcConflictArray* conflicts,
    char** error_message) {
    const std::filesystem::path operation = paths.local / workspace_name;
    io::dir(operation, error_message);
    std::string type;
    switch (type_enum) {
        case operation::type::sync:
            type = filename_sync;
            break;
        case operation::type::merge:
            type = filename_merge;
            break;
        default:
            error_message_creator("APIERROR OperationType doesn't exist", error_message);
            break;
    }
    io::file(operation / type, std::ios::binary | std::ios::trunc, error_message);
    if (conflicts->len == 0)
        return;
    std::string buffer;
    for (size_t i = 0; i < conflicts->len; i++) {
        buffer += conflicts->paths[i];
        buffer += '\n';
    }
    io::file(
        operation / filename_conflict, std::ios::binary | std::ios::trunc, buffer.data(), buffer.size(), 0,
        error_message);
}