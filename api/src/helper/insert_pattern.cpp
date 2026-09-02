#include <helper.hpp>

void insert_pattern(std::string& dest, const std::string& type) {
    size_t            content_size = dest.size();
    const std::string header       = type + ' ' + std::to_string(content_size) + '\n';
    size_t            total_len    = header.size() + content_size;

    dest.resize(total_len);
    if (content_size) std::memmove(dest.data() + total_len - content_size, dest.data(), content_size);
    std::memcpy(dest.data(), header.data(), header.size());
}