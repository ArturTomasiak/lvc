#include <core.hpp>

// each file written in object folder is turned into pattern <type> <size
// bytes>\n<content> the sha256 hash of pattern is the ID the deflation of
// pattern is written file content

void object::create(
    const std::filesystem::path& object, const std::string& type, std::string& content, std::string& id,
    char** error_message) {
    insert_pattern(content, type, nullptr);

    char content_id[65];
    sha256(content.data(), content.size(), content_id);
    id = content_id;

    if (std::filesystem::is_regular_file(object / id))
        return;

    io::file(object / id, std::ios::binary | std::ios::trunc, content.data(), content.size(), 1, error_message);
}