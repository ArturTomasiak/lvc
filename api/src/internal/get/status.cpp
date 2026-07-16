#include <internal.hpp>

std::vector<std::string> get::status(std::filesystem::path lvc) {
    std::vector<std::string> status = get::file_content_lines(lvc / "status");
    const std::vector<std::string> diff = get::diff(lvc);

    std::unordered_set<std::string> diff_entries;
    diff_entries.reserve(diff.size());
    for (const std::string& entry : diff)
        diff_entries.insert(entry);

    std::erase_if(status, [&diff_entries](const std::string& entry) {return !diff_entries.contains(entry);});

    return status;
}