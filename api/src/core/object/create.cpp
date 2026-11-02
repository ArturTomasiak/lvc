#include <core.hpp>

// each file written in object folder is turned into pattern <type> <size
// bytes>\n<content> the sha256 hash of pattern is the ID the deflation of
// pattern is written file content

void object::create(
    Paths& paths, const std::string& type, std::string& content, std::string& id, char** error_message) {
    insert_pattern(content, type, nullptr);

    char content_id[65];
    sha256(content.data(), content.size(), content_id);
    id = content_id;

    const std::filesystem::path object_path = paths.object / id;
    const std::filesystem::path object_tmp  = paths.add_pending_rename(object_path);
    if (std::filesystem::is_regular_file(object_path) || std::filesystem::is_regular_file(object_tmp)) [[unlikely]] {
        paths.pending_rename.pop_back();
        return;
    }

    io::file(object_tmp, std::ios::binary | std::ios::trunc, content.data(), content.size(), 1, error_message);
}