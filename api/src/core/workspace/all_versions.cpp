#include "core.hpp"

void branch::all_versions(
    Paths& paths, const std::string& branch_path, std::vector<std::string>& dest, char** error_message, std::string from, std::string to) {
    std::vector<std::string> versions = io::content_lines(branch_path, 1, 1, error_message);

    if (!from.empty())
        while (!versions.empty() && versions[0] != from)
            versions.erase(versions.begin());

    for (const std::string& id : versions) {
        if (!to.empty() && id == to)
            return;

        dest.emplace_back(id);

        const std::vector<std::string> content = io::content_lines(paths.object / id, 1, 1, error_message);
        if (content.size() > VERSION_BRANCH_TO)
            branch::all_versions(
                paths, paths.branch / content[VERSION_BRANCH], dest, error_message, content[VERSION_BRANCH_FROM],
                content[VERSION_BRANCH_TO]);
    }
}