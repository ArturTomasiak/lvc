#include <core.hpp>

bool workspace::is_inactive(Paths& paths, const std::string& name, std::filesystem::path& workspace) {
    const std::filesystem::path inactive = paths.workspace / workspacename_inactive;
    workspace                            = workspace_path(paths.workspace, name);
    if (inactive == workspace.parent_path())
        return 1;
    return 0;
}

bool workspace::is_inactive(const std::filesystem::path& workspace) {
    const std::filesystem::path inactive = workspace.parent_path().parent_path() / workspacename_inactive;
    if (inactive == workspace.parent_path())
        return 1;
    return 0;
}