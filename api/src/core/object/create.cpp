#include <core.hpp>

// each file written in object folder is turned into pattern <type> <size bytes>\n<content>
// the sha256 hash of pattern is the ID
// the deflation of pattern is written file content

void object::create(const std::filesystem::path& object_dir, const std::string& type, std::string& content, std::string& out_id, char** error_message) {
    insert_pattern(content, type);
    
    char id[65];
    sha256(content.data(), content.size(), id);
    out_id = id;

    if (object::exists(object_dir, id))
        return;

    io::file(object_dir / out_id, std::ios::binary, content.data(), content.size(), 1, error_message);
}