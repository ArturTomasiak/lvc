#include <core.hpp>

std::string version::latest(std::filesystem::path branch) {
    return content_first_line(branch);
}