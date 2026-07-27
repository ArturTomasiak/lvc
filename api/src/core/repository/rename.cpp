#include <core.hpp>

LvcError repository::rename(std::filesystem::path lvc, const char* name) {
    if (!file_create(lvc / NAME_NAME, std::ios::binary, name, charplen(name), 0))
        return NAME;
    return SUCCESS;
}