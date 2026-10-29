#include "core.hpp"

bool operation::conflicts_handled(
    const std::filesystem::path& root_path, const std::filesystem::path& conflict_path, char** error_message) {
    if (!std::filesystem::is_regular_file(conflict_path))
        return true;
    const std::vector<std::string> conflicts = io::content_lines(conflict_path, 0, 0, error_message);
    for (const std::string& relative : conflicts) {
        const std::filesystem::path path = root_path / relative;
        std::ifstream               file(path, std::ios::binary);
        if (!file.is_open())
            return {};
        std::string line;
        while (std::getline(file, line))
            if (line == ours_label || line == theirs_label || line == end_label)
                return false;
    }
    return true;
}