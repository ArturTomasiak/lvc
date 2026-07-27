#include <core.hpp>

std::vector<Object> workspace::all_objects(std::filesystem::path working_directory) {
    std::vector<Object> out;
    out.reserve(PREALLOCATE);

    std::error_code error;
    const std::filesystem::directory_options options = std::filesystem::directory_options::skip_permission_denied;
    std::filesystem::recursive_directory_iterator iterator(working_directory, options, error);
    const std::filesystem::recursive_directory_iterator end;

     if (error)
        return out;

    while (iterator != end) {
        error.clear();
        if (!error && iterator->is_regular_file(error)) {
            const std::filesystem::path relative_path = iterator->path().lexically_relative(working_directory);
            Object object;
            std::string buffer = content(relative_path, 0);
            char id[65]; 
            sha256(buffer.data(), buffer.size(), id);
            object.path = relative_path;
            object.type = BLOB;
            object.id = id;
            out.push_back(object);
        }
        iterator.increment(error);
    }

    return out;
}