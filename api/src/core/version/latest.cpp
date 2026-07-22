#include <core.hpp>

std::string version::latest(std::filesystem::path branch) {
    return file::content_first_line(branch);
}