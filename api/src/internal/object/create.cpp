#include <internal.hpp>

// each file written in object folder is turned into pattern <type> <size bytes>\n<content>
// the sha256 hash of pattern is the ID
// the deflation of pattern is written file content

LvcError object::create(const std::filesystem::path& object_dir, const std::string& type, std::string& content, char id[65]) {
    uint64_t content_size = content.size();
    const std::string header = type + ' ' + std::to_string(content_size) + '\n';
    uint64_t total_len = header.size() + content_size;

    content.resize(total_len);
    if (content_size)
        std::memmove(content.data() + total_len - content_size, content.data(), content_size);
    std::memcpy(content.data(), header.data(), header.size());

    sha256::charp(content.c_str(), total_len, id);
    if (object::exists(object_dir, id))
        return SUCCESS;

    if (!create::file(object_dir / id, std::ios::binary, content.c_str(), total_len, 1))
        return OBJECT_FILE_CREATE;

    return SUCCESS;
}