#include <core.hpp>

bool workspace::is_inactive(Paths& paths, std::string name) {
    const std::filesystem::path inactive  = paths.workspace / NAME_INACTIVE;
    const std::filesystem::path workspace = workspace_path(paths.workspace, name);
    if (inactive == workspace.parent_path())
        return 1;
    return 0;
}

bool workspace::is_inactive(const std::filesystem::path& workspace) {
    const std::filesystem::path inactive = workspace.parent_path().parent_path() / NAME_INACTIVE;
    if (inactive == workspace.parent_path())
        return 1;
    return 0;
}