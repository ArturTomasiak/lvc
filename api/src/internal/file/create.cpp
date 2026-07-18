#include <internal.hpp>
// TODO deflate case
bool file::create(std::filesystem::path path, std::ios_base::openmode flags) {
    std::ofstream file(path, flags);
    if (!file)
        return 0;
    file.close();
    return 1;
}

bool file::create(std::filesystem::path path, std::ios_base::openmode flags, const char* content, uint64_t length, bool deflate) {
    std::ofstream file(path, flags);
    if (!file)
        return 0;

    if (deflate) {
        uint64_t deflated_length;
        char* deflated = object::deflate(content, length, deflated_length);
        if (!deflated)
            return 0;
        file.write(deflated, deflated_length);
        free(deflated);
    }

    else
        file.write(content, length);
    
    if (!file)
        return 0;
    file.close();
    return 1;
}