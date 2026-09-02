#include <core.hpp>

void workspace::_goto(std::filesystem::path lvc, std::string workspace_name, char** error_message) {
    std::string           current       = io::content(lvc / NAME_CURRENT, 0, error_message);
    std::filesystem::path workspace_dir = lvc / NAME_WORKSPACE;
    std::filesystem::path working_dir   = lvc.parent_path();

    if (!workspace::exists(workspace_dir, workspace_name, error_message)) {
        std::string workspace_str = workspace_name;
        error_message_creator("Workspace " + workspace_str + " doesn't exist", error_message);
    }
    if (*error_message) return;
    std::filesystem::path src_operation  = workspace_dir / NAME_LOCAL / current / NAME_OPERATION;
    std::filesystem::path dest_operation = workspace_dir / NAME_LOCAL / workspace_name / NAME_OPERATION;

    version::create_tmp(lvc, src_operation, error_message);

    std::string               version_id;
    std::vector<object::info> objects;
    std::filesystem::path     object_dir;
    std::filesystem::path     tmp = dest_operation / NAME_TMP;
    if (std::filesystem::is_regular_file(tmp)) {
        object_dir = dest_operation / NAME_OBJECT;
        version_id = io::content(tmp, 0, error_message);
        objects    = version::all_objects(object_dir, version_id, {}, error_message);
    } else {
        object_dir = lvc / NAME_OBJECT;
        version_id = io::content_first_line(workspace_path(workspace_dir, workspace_name), error_message);
        objects    = version::all_objects(object_dir, version_id, {}, error_message);
    }

    std::vector<std::string>             ignore_raw = io::content_lines(working_dir / NAME_IGNORE, 0, error_message);
    std::vector<std::string>             ignore     = path_from_input(working_dir, ignore_raw, version_id, 0, error_message);
    std::unordered_set<std::string_view> ignore_set;
    ignore_set.reserve(ignore.size());
    for (const std::string& entry : ignore) ignore_set.insert(entry);

    if (*error_message) return;

    std::vector<std::filesystem::path> remove_paths;

    try {
        std::filesystem::recursive_directory_iterator iterator(working_dir);
        for (const std::filesystem::directory_entry& entry : iterator) {
            if (entry.is_directory() && entry.path().filename() == ".lvc") {
                iterator.disable_recursion_pending();
                continue;
            }
            std::filesystem::path relative = entry.path().lexically_relative(working_dir);
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

    std::sort(remove_paths.begin(), remove_paths.end(), 
    [](const std::filesystem::path& a, const std::filesystem::path& b) {
            return std::distance(a.begin(), a.end()) >
                   std::distance(b.begin(), b.end());
        });

    for (const std::filesystem::path& path : remove_paths)
        if (std::filesystem::is_regular_file(path) || std::filesystem::is_empty(path))
            std::filesystem::remove(path);

    for (object::info& object : objects) {
        std::filesystem::path path = working_dir / object.path;
        std::filesystem::create_directories(path.parent_path());
        if (object.type == object::type::blob) {
            std::string buffer = io::content(object_dir / object.id, 1, error_message);
            size_t      pos    = buffer.find('\n');
            io::file(working_dir / path, std::ios::binary, buffer.data() + pos + 1, buffer.size() - pos - 1, 0, error_message);
        }
        else
            std::filesystem::create_directories(path);
    }

    io::file(lvc / NAME_CURRENT, std::ios::binary, workspace_name.c_str(), workspace_name.size(), 0, error_message);
}

void workspace::_default(std::filesystem::path lvc, const char* workspace_name, char** error_message) {
    std::filesystem::path workspace_dir = lvc / NAME_WORKSPACE;

    if (!workspace::exists(workspace_dir, workspace_name, error_message)) {
        std::string workspace_str = workspace_name;
        error_message_creator("Workspace " + workspace_str + " doesn't exist", error_message);
    }
    if (*error_message) return;

    if (workspace::is_inactive(workspace_dir, workspace_name)) {
        error_message_creator("Cannot make inactive workspace default", error_message);
        return;
    }

    io::file(lvc / NAME_DEFAULT, std::ios::binary, workspace_name, charplen(workspace_name), 0, error_message);
}