#include <core.hpp>

// Revert specified files or all files if no input is given in the working
// directory to the state of the version_id. Does not touch files specified by
// lvc.ignore.

void version::revert(Paths& paths, std::string version_id, std::vector<std::string>& input_raw, char** error_message) {
    if (!std::filesystem::is_regular_file(paths.object / version_id)) {
        error_message_creator("Invalid version ID " + version_id, error_message);
        return;
    }

    std::vector<std::string> ignore_raw = io::content_lines(paths.ignore, 0, error_message);
    std::vector<std::string> ignore     = path_from_input(paths.root, ignore_raw, version_id, 0, error_message);
    if (*error_message)
        return;

    std::unordered_set<std::string_view> ignore_set;
    ignore_set.reserve(ignore.size());
    for (const std::string& entry : ignore)
        ignore_set.insert(entry);

    std::vector<object::info> objects = version::all_objects(paths.object, version_id, ignore_set, error_message);

    if (input_raw.empty())
        input_raw.push_back("/");

    std::vector<std::string> input = path_from_input(paths.object, input_raw, version_id, 0, error_message);
    if (*error_message)
        return;

    std::unordered_set<std::string_view> input_set;
    input_set.reserve(input.size());
    for (const std::string& entry : input)
        input_set.insert(entry);

    for (const std::string& entry : ignore)
        input_set.erase(entry);

    std::erase_if(
        objects, [&input_set](const object::info& entry) { return !input_set.contains(entry.path.string()); });

    std::unordered_set<std::string> objects_set;
    objects_set.reserve(input.size());
    for (const object::info& entry : objects)
        objects_set.insert(entry.path);

    try {
        std::filesystem::recursive_directory_iterator iterator(paths.root);
        for (const std::filesystem::directory_entry& entry : iterator) {
            std::filesystem::path relative = entry.path().lexically_relative(paths.root);
            if (!input_set.contains(relative.string()))
                continue;
            if (!objects_set.contains(relative.string()))
                std::filesystem::remove_all(entry.path());
        }
    } catch (const std::filesystem::filesystem_error& error) {
        error_message_creator("Directory iteration failure", error_message);
        return;
    }

    for (object::info& object : objects) {
        if (object.type == object::type::blob) {
            std::string buffer = io::content(paths.object / object.id, 1, error_message);
            size_t      pos    = buffer.find('\n');
            io::file(
                paths.root / object.path, std::ios::binary, buffer.data() + pos + 1, buffer.size() - pos - 1, 0,
                error_message);
        }
    }
}