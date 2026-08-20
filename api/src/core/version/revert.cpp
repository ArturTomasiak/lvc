#include <core.hpp>

// Revert specified files or all files if no input is given in the working directory to the state of the version_id.
// Does not touch files specified by lvc.ignore.

LvcError version::revert(std::filesystem::path lvc, std::string version_id, std::vector<std::string> input_raw) {
    std::filesystem::path working_dir = lvc.parent_path();
    std::filesystem::path object_dir  = lvc / NAME_OBJECT;
    LvcError out = SUCCESS;
    
    if (input_raw.empty())
        input_raw.emplace_back("/");
    std::vector<std::string> input = path_from_input(working_dir, input, "", out);
    if (out)
        return out;

    std::vector<std::string> ignore_raw = io::content_lines(working_dir / NAME_IGNORE, 0);
    std::vector<std::string> ignore     = path_from_input(working_dir, ignore_raw, "", out);
    if (out)
        return out;

    std::unordered_set<std::string> revert_paths;
    revert_paths.reserve(input.size());
    for (const std::string& entry : input)
        revert_paths.insert(entry);

    for (const std::string& entry : ignore)
        revert_paths.erase(entry);

    std::vector<Object> objects = version::all_objects(object_dir, version_id, revert_paths);
    
    return out;
}