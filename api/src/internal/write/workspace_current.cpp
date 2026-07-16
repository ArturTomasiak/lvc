#include <internal.hpp>

LvcError write::workspace_current(std::filesystem::path lvc, const char* category_name, const char* workspace_name) {
    if (!exists::workspace(lvc / NAME_WORKSPACE, workspace_name))
        return WORKSPACE_NOT_EXISTS;
    char* category_slash_workspace = charpcombslash(category_name, workspace_name);
    if (!category_slash_workspace)
        return MEMORY_ALLOCATION_FAILED;
    if (!write::file(lvc / NAME_CURRENT, std::ios::binary, category_slash_workspace, charplen(category_slash_workspace)))
        return CURRENT;
    free(category_slash_workspace);
    return SUCCESS;
}