#include "core.hpp"

void workspace::all_versions(
    Paths& paths, const std::string& workspace_path, std::vector<std::string>& dest, char** error_message,
    std::string from, std::string to) {
    std::vector<std::string> versions = io::content_lines(workspace_path, 1, 1, error_message);

    if (!from.empty())
        while (!versions.empty() && versions[0] != from)
            versions.erase(versions.begin());

    for (const std::string& id : versions) {
        if (!to.empty() && id == to)
            return;

        dest.emplace_back(id);

        const std::vector<std::string> content = io::content_lines(paths.object / id, 1, 1, error_message);
        if (content.size() > VERSION_WORKSPACE_TO)
            workspace::all_versions(
                paths, paths.workspace / content[VERSION_WORKSPACE], dest, error_message,
                content[VERSION_WORKSPACE_FROM], content[VERSION_WORKSPACE_TO]);
    }
}