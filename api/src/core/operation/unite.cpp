#include "core.hpp"

LvcConflictArray* operation::unite(
    Paths& paths, std::string src_workspace, std::string dest_workspace, const char* author, char** error_message) {
    LvcConflictArray* out = NULL;
    if (workspace::is_inactive(src_workspace)) {
        error_message_creator(src_workspace + IS_INACTIVE_STRING, error_message);
        return out;
    }
    if (workspace::is_inactive(dest_workspace)) {
        error_message_creator(dest_workspace + IS_INACTIVE_STRING, error_message);
        return out;
    }
    std::string message = "unite with workspace: ";
    message += src_workspace;
    version::create(paths, message.c_str(), author, "", error_message);
    return out;
}