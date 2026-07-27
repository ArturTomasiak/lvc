#include <core.hpp>

LvcError workspace::_goto(std::filesystem::path lvc, const char* workspace_name) {
    std::filesystem::path workspace_dir = lvc / NAME_WORKSPACE;
    if (!workspace::exists(workspace_dir, workspace_name))
        return WORKSPACE_NOT_EXISTS;
    if (workspace::is_inactive(workspace_dir, workspace_name))
        return GOTO_INACTIVE;
    if (!io::file(lvc / NAME_CURRENT, std::ios::binary, workspace_name, charplen(workspace_name), 0))
        return CURRENT;
    return SUCCESS;
}

LvcError workspace::_default(std::filesystem::path lvc, const char* workspace_name) {
    std::filesystem::path workspace_dir = lvc / NAME_WORKSPACE;
    if (!workspace::exists(workspace_dir, workspace_name))
        return WORKSPACE_NOT_EXISTS;
    if (workspace::is_inactive(workspace_dir, workspace_name))
        return DEFAULT_INACTIVE;
    if (!io::file(lvc / NAME_DEFAULT, std::ios::binary, workspace_name, charplen(workspace_name), 0))
        return DEFAULT;
    return SUCCESS;
}