#include <internal.hpp>

bool write::file(std::filesystem::path path, std::ios_base::openmode flags, const char* content) {
    std::ofstream file(path, flags);
    if (!file)
        return 0;
    if (content)
        file.write(content, std::strlen(content));
    file.close();
    return 1;
}

LvcError write::workspace_current(std::filesystem::path lvc, const char* workspace_name) {
    if (!exists::workspace(lvc, workspace_name, 0))
        return WORKSPACE_NOT_EXISTS;
    if (! write::file(lvc / "current", std::ios::binary, workspace_name));
        return CURRENT;
    return SUCCESS;
}

LvcError write::workspace_default(std::filesystem::path lvc, const char* workspace_name) {
    if (!exists::workspace(lvc, workspace_name, 0))
        return WORKSPACE_NOT_EXISTS;
    if (! write::file(lvc / "default", std::ios::binary, workspace_name));
        return DEFAULT;
    return SUCCESS;
}