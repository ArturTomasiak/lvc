#include <internal.hpp>

LvcError write::repository_name(std::filesystem::path lvc, const char* name) {
    if (!write::file(lvc / "name", std::ios::binary, name, charplen(name)))
        return NAME;
    return SUCCESS;
}