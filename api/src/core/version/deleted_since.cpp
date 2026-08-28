#include <core.hpp>

std::vector<std::filesystem::path> version::deleted_since(std::filesystem::path object_dir, std::filesystem::path working_directory, std::string id, char** error_message) {
    std::vector<std::filesystem::path> out;
    out.reserve(PREALLOCATE);

    std::vector<object::info> version = version::all_objects(object_dir, id, {}, error_message);
    std::unordered_set<std::filesystem::path> working_files;
    
try {
    std::filesystem::recursive_directory_iterator iterator(working_directory);
    for (const std::filesystem::directory_entry& entry : iterator)
        working_files.insert(entry.path().lexically_relative(working_directory));
}   
catch(const std::filesystem::filesystem_error& error) {
    error_message_creator("Directory iteration failure", error_message);
    return {};
}

    for (const object::info& object : version)
        if (!object.path.empty() && !working_files.contains(object.path))
            out.push_back(object.path);

    return out;
}