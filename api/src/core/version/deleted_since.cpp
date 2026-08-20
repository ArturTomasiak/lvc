#include <core.hpp>

std::vector<std::filesystem::path> version::deleted_since(std::filesystem::path object_dir, std::filesystem::path working_directory, std::string id, LvcError& err) {
    err = SUCCESS;

    std::vector<std::filesystem::path> out;
    out.reserve(PREALLOCATE);

    std::vector<Object> version = version::all_objects(object_dir, id, {});
    std::unordered_set<std::filesystem::path> working_files;
    
try {
    std::filesystem::recursive_directory_iterator iterator(working_directory);
    for (const std::filesystem::directory_entry& entry : iterator) {
        if (!entry.is_regular_file())
            continue;
        working_files.insert(entry.path().lexically_relative(working_directory));
    }
}   
catch(const std::filesystem::filesystem_error& error) {
    err = WORKING_DIR_ITERATION_FAILED;
    return {};
}

    for (const Object& object : version)
        if (object.type != TREE && !working_files.contains(object.path))
            out.push_back(object.path);

    return out;
}