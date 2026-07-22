#include <core.hpp>

LvcError workspace::_goto(std::filesystem::path lvc, const char* category_name, const char* workspace_name) {
    if (!workspace::exists(lvc / NAME_WORKSPACE, workspace_name))
        return WORKSPACE_NOT_EXISTS;
    char* category_slash_workspace = charpcombslash(category_name, workspace_name);
    if (!category_slash_workspace)
        return MEMORY_ALLOCATION_FAILED;
    if (!file::create(lvc / NAME_CURRENT, std::ios::binary, category_slash_workspace, charplen(category_slash_workspace), 0))
        return CURRENT;
    free(category_slash_workspace);
    return SUCCESS;
}

LvcError workspace::_default(std::filesystem::path lvc, const char* category_name, const char* workspace_name) {
    if (!workspace::exists(lvc / NAME_WORKSPACE, workspace_name))
        return WORKSPACE_NOT_EXISTS;
    char* category_slash_workspace = charpcombslash(category_name, workspace_name);
    if (!category_slash_workspace)
        return MEMORY_ALLOCATION_FAILED;
    if (!file::create(lvc / NAME_DEFAULT, std::ios::binary, category_slash_workspace, charplen(category_slash_workspace), 0))
        return DEFAULT;
    free(category_slash_workspace);
    return SUCCESS;
}