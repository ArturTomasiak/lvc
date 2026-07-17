#include <internal.hpp>

std::vector<std::string> sha256::working_directory(std::filesystem::path working_dir) {
    std::vector<std::string> out;
    for (const std::filesystem::directory_entry& entry : std::filesystem::recursive_directory_iterator(working_dir)) {
        if (!entry.is_regular_file())
            continue;
        std::string content = get::file_content(entry.path(), 0);
        char id[65];
        sha256::charp(content.c_str(), content.size(), id);
        out.push_back(id);
    }
    return out;
}