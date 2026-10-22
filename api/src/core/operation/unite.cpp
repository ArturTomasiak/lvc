#include "core.hpp"

LvcConflictArray* operation::unite(
    Paths& paths, std::string src_workspace, std::string dest_workspace, const char* author, char** error_message) {
    LvcConflictArray* out = NULL;

    std::string message = "unite with workspace: ";
    message += src_workspace;
    version::create(paths, message.c_str(), author, "", error_message);
    return out;
}