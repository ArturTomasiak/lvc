#include <internal.hpp>

bool write::file(std::filesystem::path path, std::ios_base::openmode flags) {
    std::ofstream file(path, flags);
    if (!file)
        return 0;
    file.close();
    return 1;
}

bool write::file(std::filesystem::path path, std::ios_base::openmode flags, const char* content) {
    std::ofstream file(path, flags);
    if (!file)
        return 0;
    file.write(content, std::strlen(content));
    file.close();
    return 1;
}