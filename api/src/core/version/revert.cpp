#include <core.hpp>

// Revert specified files or all files if no input is given in the working directory to the state of the version_id.
// Does not touch files specified by lvc.ignore.

LvcError version::revert(std::filesystem::path lvc, std::string version_id, std::vector<std::string> input_raw) {
    std::filesystem::path working_dir = lvc.parent_path();
    std::filesystem::path object_dir  = lvc / NAME_OBJECT;
    LvcError out = SUCCESS;

    std::vector<std::string> ignore_raw = io::content_lines(working_dir / NAME_IGNORE, 0);
    std::vector<std::string> ignore     = path_from_input(working_dir, ignore_raw, version_id, 0, out);
    if (out)
        return out;

    std::unordered_set<std::string> ignore_set;
    ignore_set.reserve(ignore.size());
    for (const std::string& entry : ignore)
        ignore_set.insert(entry);

    std::vector<Object> objects = version::all_objects(object_dir, version_id, ignore_set);
    
    if (input_raw.empty()) 
        input_raw.push_back("/");

    std::vector<std::string> input = path_from_input(working_dir, input, version_id, 0, out);
    if (out)
        return out;

    std::unordered_set<std::string> input_set;
    input_set.reserve(input.size());
    for (const std::string& entry : input)
        input_set.insert(entry);

    std::erase_if(objects, [&input_set](const Object& entry) {return !input_set.contains(entry.path);});

try {
    std::filesystem::recursive_directory_iterator iterator(working_dir);
    for (const std::filesystem::directory_entry& entry : iterator) {
        if (!input_set.contains(entry.path()))
            continue;

        bool object_exists = 0;
        for (Object& object : objects) {
            if (object.path == entry.path()) {
                object_exists = 1;
                if (entry.is_regular_file()) {
                    std::string buffer = io::content(object_dir / object.id, 1);
                    size_t pos = buffer.find('\n');
                    io::file(working_dir / entry.path(), std::ios::binary, buffer.data() + pos + 1, buffer.size() - pos - 1, 0);
                }
            }
        }
        if (!object_exists)
            std::filesystem::remove_all(entry.path());
    }
}   
catch(const std::filesystem::filesystem_error& error) {
    return WORKING_DIR_ITERATION_FAILED;
}

    return out;
}