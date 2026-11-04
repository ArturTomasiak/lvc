#include <core.hpp>

std::string category::get(const std::string& pattern) {
    const auto first = pattern.find(' ');
    const auto start = pattern.find(' ', first + 1) + 1;
    const auto end   = pattern.find('\n', start);
    return pattern.substr(start, end - start);
}