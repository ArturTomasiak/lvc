#include <core.hpp>

void workspace::_goto(Paths& paths, std::string dest_name, char** error_message) {
    const std::string     src_name  = io::content(paths.current, 0, 0, error_message);
    std::filesystem::path dest_path = paths.workspace / dest_name;
    if (!std::filesystem::is_regular_file(dest_path))
        error_message_creator("Workspace " + dest_name + " doesn't exist", error_message);
    if (dest_name == src_name)
        error_message_creator("Already in " + dest_name, error_message);
    CHECKERROR(error_message);
    if (src_name == dest_name)
        error_message_creator("Current workspace is already " + dest_name, error_message);
    const std::filesystem::path     src_operation  = paths.local / src_name;
    const std::filesystem::path     dest_operation = paths.local / dest_name;
    const std::vector<std::string>  dest_versions  = io::content_lines(dest_path, 1, 1, error_message);
    std::string                     dest_id        = dest_versions.empty() ? "" : dest_versions[0];
    const std::vector<std::string>  src_versions   = io::content_lines(paths.workspace / src_name, 1, 1, error_message);
    const std::string               src_id         = src_versions.empty() ? "" : src_versions[0];
    const std::filesystem::path     dest_tmp       = dest_operation / filename_tmp;
    const std::vector<std::string>  ignore_raw     = io::content_lines(paths.ignore, 0, 0, error_message);
    const std::vector<std::string>  ignore         = paths.resolve(ignore_raw, dest_id, 0, 1);
    const std::vector<object::info> src_objects    = version::all_objects(paths.object, src_id, {}, error_message);
    std::vector<object::info>       dest_objects   = version::all_objects(paths.object, dest_id, {}, error_message);
    bool                            remove_tmp     = false;
    std::unordered_set<std::string_view> ignore_set;
    ignore_set.reserve(ignore.size());
    for (const std::string& entry : ignore)
        ignore_set.insert(entry);

    if (src_id.empty() || workspace::changed_since_ver(paths, src_objects, error_message))
        version::create_tmp(paths, src_operation, ignore_set, error_message);

    std::filesystem::path dest_object_dir;
    if (std::filesystem::is_regular_file(dest_tmp)) {
        dest_id         = io::content(dest_tmp, 0, 0, error_message);
        dest_object_dir = dest_operation / foldername_object;
        dest_objects    = version::all_objects(dest_object_dir, dest_id, {}, error_message);
        remove_tmp      = true;
    } else {
        dest_object_dir = paths.object;
    }

    {
        std::vector<std::filesystem::path> remove_dir;
        remove_dir.reserve(PREALLOCATE);

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
                if (std::filesystem::is_directory(entry.path()))
                    remove_dir.push_back(std::move(entry.path()));
                else
                    paths.move_pending({.type = PendingType::remove, .dest = std::move(entry.path())});
            }
        } catch (const std::filesystem::filesystem_error& error) {
            error_message_creator("Directory iteration failure", error_message);
            return;
        }

        for (size_t i = remove_dir.size(); i > 0;)
            paths.move_pending({.type = PendingType::remove_conditional, .dest = remove_dir[--i]});
    }

    CHECKERROR(error_message);
    for (object::info& object : dest_objects) {
        const std::filesystem::path path = paths.root / object.path;

        paths.move_pending({.type = PendingType::create_directories_conditional, .dest = path.parent_path()});
        if (object.type == object::type::blob) {
            const Pending pending = {.type = PendingType::rename, .src = paths.get_tmp_file(), .dest = path};
            std::string   buffer  = io::content(dest_object_dir / object.id, 1, 1, error_message);
            io::file(pending.src, std::ios::binary | std::ios::trunc, buffer.data(), buffer.size(), 0, error_message);
            paths.move_pending(pending);
        } else if (object.type == object::type::tree)
            paths.move_pending({.type = PendingType::create_directory_conditional, .dest = path});
    }

    const Pending pending = {.type = PendingType::rename, .src = paths.get_tmp_file(), .dest = paths.current};
    io::file(pending.src, std::ios::binary | std::ios::trunc, dest_name.c_str(), dest_name.size(), 0, error_message);
    paths.move_pending(pending);

    if (remove_tmp) {
        paths.move_pending({.type = PendingType::remove, .dest = dest_tmp});
        paths.move_pending({.type = PendingType::remove_all, .dest = dest_object_dir});
    }
    CHECKERROR(error_message);
}