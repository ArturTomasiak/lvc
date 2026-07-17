#include <internal.hpp>

LvcError create::repository_name(std::filesystem::path lvc, const char* name) {
    if (!create::file(lvc / NAME_NAME, std::ios::binary, name, charplen(name), 0))
        return NAME;
    return SUCCESS;
}