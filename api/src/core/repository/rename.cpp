#include <core.hpp>

LvcError repository::rename(std::filesystem::path lvc, const char* name) {
    RETURN_ERR(io::file(lvc / NAME_NAME, std::ios::binary, name, charplen(name), 0));
    return SUCCESS;
}