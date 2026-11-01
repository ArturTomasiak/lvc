#include <core.hpp>

void workspace::_goto(Paths& paths, std::string workspace_name, char** error_message) {
    const std::string              current        = io::content(paths.current, 0, 0, error_message);
    const std::filesystem::path    workspace      = paths.workspace / workspace_name;
    const std::filesystem::path    src_operation  = paths.local / current;
    const std::filesystem::path    dest_operation = paths.local / workspace_name;
    const std::filesystem::path    tmp            = dest_operation / filename_tmp;
    const std::vector<std::string> ignore_raw     = io::content_lines(paths.ignore, 0, 0, error_message);
    bool                           remove_tmp     = false;

    if (!std::filesystem::is_regular_file(workspace))
        error_message_creator("Workspace " + workspace_name + " doesn't exist", error_message);
    if (workspace_name == current)
        error_message_creator("Already in " + workspace_name, error_message);
    ERRORCHECK(error_message);

    std::filesystem::path object_dir;
    std::string           version_id;
    if (std::filesystem::is_regular_file(tmp)) {
        version_id = io::content(tmp, 0, 0, error_message);
        object_dir = dest_operation / foldername_object;
        remove_tmp = true;
    } else {
        object_dir                                       = paths.object;
        const std::vector<std::string> workspace_content = io::content_lines(workspace, 1, 1, error_message);
        version_id                                       = workspace_content.empty() ? "" : workspace_content[0];
    }
    std::vector<object::info> objects = version::all_objects(object_dir, version_id, {}, error_message);

    const std::vector<std::string>       ignore = paths.resolve(ignore_raw, version_id, 0, 1, error_message);
    std::unordered_set<std::string_view> ignore_set;
    ignore_set.reserve(ignore.size());
    for (const std::string& entry : ignore)
        ignore_set.insert(entry);

    if (version_id.empty() || workspace::changed_since_ver(paths, objects, error_message))
        version::create_tmp(paths, src_operation, ignore_set, error_message);

    ERRORCHECK(error_message);

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
            paths.pending_delete.push_back(entry.path());
        }
    } catch (const std::filesystem::filesystem_error& error) {
        error_message_creator("Directory iteration failure", error_message);
        return;
    }

    for (object::info& object : objects) {
        if (object.type == object::type::blob) {
            PendingRename pending = {.tmp = paths.tmp / object.id, .dest = paths.root / object.path};
            std::string   buffer  = io::content(object_dir / object.id, 1, 1, error_message);
            io::file(pending.tmp, std::ios::binary | std::ios::trunc, buffer.data(), buffer.size(), 0, error_message);
            paths.pending_rename.emplace_back(std::move(pending));
        } else
            paths.pending_directory.emplace_back(paths.root / object.path);
    }

    PendingRename pending = {.tmp = paths.tmp / filename_current, .dest = paths.current};
    io::file(
        pending.tmp, std::ios::binary | std::ios::trunc, workspace_name.c_str(), workspace_name.size(), 0,
        error_message);
    paths.pending_rename.emplace_back(std::move(pending));

    if (remove_tmp) {
        paths.pending_delete.push_back(tmp);
        paths.pending_delete.push_back(object_dir);
    }
}