#include <core.hpp>

std::vector<Object> workspace::all_objects(std::filesystem::path working_directory, LvcError& err) {
    std::vector<Object> out;
    out.reserve(PREALLOCATE);

try {
    std::filesystem::recursive_directory_iterator iterator(working_directory);
    std::filesystem::recursive_directory_iterator end;

    for (const std::filesystem::directory_entry& entry = *iterator; iterator != end; iterator++) {
        if (entry.is_directory() &&
            entry.path().filename() == ".lvc") {
            iterator.disable_recursion_pending();
            continue;
        }

        if (!entry.is_regular_file())
            continue;

        Object object;
        object.path = entry.path().lexically_relative(working_directory);
        object.type = BLOB;

        std::string buffer = io::content(entry.path(), 0);
        insert_pattern(buffer, TYPE_BLOB);

        char id[65];
        sha256(buffer.data(), buffer.size(), id);

        object.id = id;
        out.push_back(std::move(object));
    }
}   
catch(const std::filesystem::filesystem_error& error) {
    err = WORKING_DIR_ITERATION_FAILED;
    return out;
}

    err = SUCCESS;
    return out;
}