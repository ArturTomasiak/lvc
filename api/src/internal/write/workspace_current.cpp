#include <internal.hpp>

LvcError write::workspace_current(std::filesystem::path lvc, const char* workspace_name) {
    if (!exists::workspace(lvc, workspace_name))
        return WORKSPACE_NOT_EXISTS;
    if (!write::file(lvc / "current", std::ios::binary, workspace_name))
        return CURRENT;
    return SUCCESS;
}