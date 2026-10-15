#include "core.hpp"

void operation::create(
    Paths& paths, const std::string& workspace_name, operation::type type_enum, std::vector<MergeResult>& merge,
    char** error_message) {
    const std::filesystem::path operation = paths.local / workspace_name / foldername_operation;
    io::dir(operation, error_message);
    std::string type;
    switch (type_enum) {
        case operation::type::sync:
            type = filename_sync;
            break;
        case operation::type::unite:
            type = filename_unite;
            break;
        case operation::type::insert:
            type = filename_insert;
            break;
        default:
            error_message_creator("APIERROR OperationType doesn't exist", error_message);
            break;
    }
    io::file(operation / type, std::ios::binary | std::ios::trunc, error_message);
    if (merge.empty())
        return;
    std::string buffer;
    for (MergeResult& entry : merge) {
        buffer += entry.path;
        buffer += '\n';
    }
    io::file(
        operation / filename_conflict, std::ios::binary | std::ios::trunc, buffer.data(), buffer.size(), 0,
        error_message);
}