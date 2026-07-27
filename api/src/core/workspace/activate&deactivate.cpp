#include <core.hpp>

LvcError workspace::deactivate(std::filesystem::path workspace_dir, const char* workspace_name) {
    std::filesystem::path oldp = workspace_path(workspace_dir, workspace_name);
    std::filesystem::path newp = workspace_dir / NAME_INACTIVE / workspace_name;
    std::error_code ec;
    std::filesystem::rename(oldp, newp, ec);
    return ec ? WORKSPACE_DEACTIVATE : SUCCESS;
}

LvcError workspace::activate(std::filesystem::path workspace_dir, const char* workspace_name, const char* category_name) {
    // TODO SEARCH FOR VERSION THAT HAS THIS WORKSPACE AS A RESULT OF INSERT AND REMOVE THE WORKSPACE
    if (!category::exists(workspace_dir, category_name))
        return CATEGORY_NOT_EXISTS;
    std::filesystem::path oldp = workspace_dir / NAME_INACTIVE / workspace_name;
    std::filesystem::path newp = workspace_dir / category_name / workspace_name;

    std::error_code ec;
    std::filesystem::rename(oldp, newp, ec);
    return ec ? WORKSPACE_DEACTIVATE : SUCCESS;
}