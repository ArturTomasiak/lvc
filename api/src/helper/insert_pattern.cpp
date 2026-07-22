#include <helper.hpp>

void insert_pattern(std::string& content, const std::string& type) {
    uint64_t content_size = content.size();
    const std::string header = type + ' ' + std::to_string(content_size) + '\n';
    uint64_t total_len = header.size() + content_size;

    content.resize(total_len);
    if (content_size)
        std::memmove(content.data() + total_len - content_size, content.data(), content_size);
    std::memcpy(content.data(), header.data(), header.size());
}