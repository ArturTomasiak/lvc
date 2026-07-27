#include <core.hpp>

std::vector<std::string> version::status(std::filesystem::path lvc) {
    std::vector<std::string> status = io::content_lines(lvc / NAME_STATUS, 0);
    const std::vector<std::string> diff = version::diff(lvc);

    std::unordered_set<std::string> diff_entries;
    diff_entries.reserve(diff.size());
    for (const std::string& entry : diff)
        diff_entries.insert(entry);

    std::erase_if(status, [&diff_entries](const std::string& entry) {return !diff_entries.contains(entry);});

    return status;
}

std::vector<std::string> version::status_all(std::filesystem::path lvc) {
    return io::content_lines(lvc / NAME_STATUS, 0);
}