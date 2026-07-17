#include <internal.hpp>

std::string get::latest_version(std::filesystem::path branch) {
    return get::file_content_first_line(branch, 0);
}