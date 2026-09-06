#include <core.hpp>

static bool changed_since_ver(Paths& paths, std::vector<object::info>& objects_version, char** error_message) {
    std::vector<object::info> objects_working_dir = workspace::all_objects(paths, error_message);

    std::unordered_set<std::string> version_ids_and_paths;
    version_ids_and_paths.reserve(objects_version.size());
    for (const object::info& object : objects_version) {
        version_ids_and_paths.emplace(object.id);
        version_ids_and_paths.emplace(object.path.string());
    }

    for (const object::info& object : objects_working_dir)
        if (!version_ids_and_paths.contains(object.id) || !version_ids_and_paths.contains(object.path.string()))
            return true;
    return false;
}

void workspace::_goto(Paths& paths, std::string workspace_name, char** error_message) {
    std::string current = io::content(paths.current, 0, error_message);
    if (!workspace::exists(paths, workspace_name, error_message))
        error_message_creator("Workspace " + workspace_name + " doesn't exist", error_message);
    if (*error_message)
        return;
    if (current == workspace_name)
        error_message_creator("Current workspace is already " + workspace_name, error_message);
    std::filesystem::path src_operation  = paths.local / current;
    std::filesystem::path dest_operation = paths.local / workspace_name;
    std::string version_id = io::content_first_line(workspace_path(paths.workspace, workspace_name), error_message);

    std::vector<object::info> objects = version::all_objects(paths.object, version_id, {}, error_message);

    std::filesystem::path tmp = dest_operation / NAME_TMP;

    if (version_id.empty() || changed_since_ver(paths, objects, error_message))
        version::create_tmp(paths, src_operation, error_message);

    std::filesystem::path object_dir;
    if (std::filesystem::is_regular_file(tmp)) {
        object_dir = dest_operation / NAME_OBJECT;
        version_id = io::content(tmp, 0, error_message);
        objects    = version::all_objects(object_dir, version_id, {}, error_message);
    } else {
        object_dir = paths.object;
    }

    std::vector<std::string>             ignore_raw = io::content_lines(paths.ignore, 0, error_message);
    std::vector<std::string>             ignore = path_from_input(paths.root, ignore_raw, version_id, 0, error_message);
    std::unordered_set<std::string_view> ignore_set;
    ignore_set.reserve(ignore.size());
    for (const std::string& entry : ignore)
        ignore_set.insert(entry);

    if (*error_message)
        return;

    std::vector<std::filesystem::path> remove_paths;

    try {
        std::filesystem::recursive_directory_iterator iterator(paths.root);
        for (const std::filesystem::directory_entry& entry : iterator) {
            if (entry.is_directory() && entry.path().filename() == ".lvc") {
                iterator.disable_recursion_pending();
                continue;
            }
            std::filesystem::path relative = entry.path().lexically_relative(paths.root);
            if (ignore_set.contains(relative.string())) {
                if (entry.is_directory())
                    iterator.disable_recursion_pending();

                continue;
            }
            remove_paths.push_back(entry.path());
        }
    } catch (const std::filesystem::filesystem_error& error) {
        error_message_creator("Directory iteration failure", error_message);
        return;
    }

    std::sort(
        remove_paths.begin(), remove_paths.end(), [](const std::filesystem::path& a, const std::filesystem::path& b) {
            return std::distance(a.begin(), a.end()) > std::distance(b.begin(), b.end());
        });

    for (const std::filesystem::path& path : remove_paths)
        if (std::filesystem::is_regular_file(path) || std::filesystem::is_empty(path))
            std::filesystem::remove(path);

    for (object::info& object : objects) {
        std::filesystem::path path = paths.root / object.path;
        std::filesystem::create_directories(path.parent_path());
        if (object.type == object::type::blob) {
            std::string buffer = io::content(object_dir / object.id, 1, error_message);
            size_t      pos    = buffer.find('\n');
            io::file(
                paths.root / path, std::ios::binary, buffer.data() + pos + 1, buffer.size() - pos - 1, 0,
                error_message);
        } else
            std::filesystem::create_directories(path);
    }

    io::file(paths.current, std::ios::binary, workspace_name.c_str(), workspace_name.size(), 0, error_message);
}