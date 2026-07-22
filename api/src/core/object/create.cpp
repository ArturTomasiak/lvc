#include <core.hpp>

// each file written in object folder is turned into pattern <type> <size bytes>\n<content>
// the sha256 hash of pattern is the ID
// the deflation of pattern is written file content

LvcError object::create(const std::filesystem::path& object_dir, const std::string& type, std::string& content, char id[65]) {
    object::insert_pattern(content, type);
    sha256(content.data(), content.size(), id);
    if (object::exists(object_dir, id))
        return SUCCESS;

    if (!file::create(object_dir / id, std::ios::binary, content.data(), content.size(), 1))
        return OBJECT_FILE_CREATE;

    return SUCCESS;
}