#include "core.hpp"

bool operation::conflicts_handled(Paths& paths, const std::string& workspace_name, char** error_message) {
    const std::filesystem::path operation = paths.local / workspace_name / foldername_operation / filename_conflict;
    if (!std::filesystem::is_regular_file(operation))
        return true;
    const std::vector<std::string> conflicts = io::content_lines(operation, 0, error_message);
    for (const std::string& relative : conflicts) {
        const std::filesystem::path    path   = paths.root / relative;
        const std::vector<std::string> buffer = io::content_lines(path, 0, error_message);
        for (const std::string& line : buffer)
            if (line == ours_label || line == theirs_label)
                return false;
    }
    return true;
}