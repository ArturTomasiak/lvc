#include "core.hpp"

static void validate_local(std::filesystem::path& path, char** error_message) {
    if (!std::filesystem::is_directory(path / ".lvc"))
        error_message_creator("Clone has no .lvc folder", error_message);
}

static void clone_local(Paths& paths, std::filesystem::path path, bool clone_versioning, char** error_message) {
    Paths             their_paths(path.c_str(), error_message);
    const std::string default_workspace_name = io::content(their_paths.lvc / filename_default, 0, 0, error_message);
    const std::vector<std::string> default_workspace =
        io::content_lines(their_paths.workspace / default_workspace_name, 1, 1, error_message);
    const std::string version = default_workspace.empty() ? std::string{} : default_workspace[0];

    if (!std::filesystem::is_directory(their_paths.lvc)) {
        error_message_creator("Clone has no .lvc folder", error_message);
        return;
    }

    if (clone_versioning) {
        paths.add_pending({.type = PendingType::create_directory, .dest = paths.lvc});
        try {
            std::filesystem::recursive_directory_iterator iterator(their_paths.lvc);
            for (const std::filesystem::directory_entry& entry : iterator) {
                const std::filesystem::path dest = paths.root / entry.path().lexically_relative(their_paths.root);
                if (entry.is_directory()) {
                    if (entry.path().filename() == foldername_tmp) {
                        iterator.disable_recursion_pending();
                        continue;
                    }
                    paths.add_pending({.type = PendingType::create_directory, .dest = dest});
                    continue;
                }
                const std::filesystem::path tmp = paths.tmp_file();
                std::error_code             ec;
                std::filesystem::copy(entry.path(), tmp, std::filesystem::copy_options::overwrite_existing, ec);
                paths.add_pending({.type = PendingType::rename, .src = tmp, .dest = dest});
                if (ec) {
                    error_message_creator("Failed to copy the lvc folder", error_message);
                    return;
                }
            }
        } catch (const std::filesystem::filesystem_error& error) {
            error_message_creator(error.what(), error_message);
            return;
        }

        const std::filesystem::path tmp_current = paths.tmp_file();
        io::file(
            tmp_current, std::ios::binary | std::ios::trunc, default_workspace_name.c_str(), default_workspace_name.size(), 0,
            error_message);
        paths.add_pending({.type = PendingType::rename, .src = tmp_current, .dest = paths.current});
    }

    if (!version.empty()) {
        const std::vector<object::info> objects = version::all_objects(their_paths.object, version, {}, error_message);
        for (const object::info& object : objects) {
            const std::filesystem::path path = paths.root / object.path;
            if (object.type == object::type::blob) {
                const std::filesystem::path tmp    = paths.tmp_file();
                std::string                 buffer = io::content(their_paths.object / object.id, 1, 1, error_message);
                io::file(tmp, std::ios::binary | std::ios::trunc, buffer.data(), buffer.size(), 0, error_message);
                paths.add_pending({.type = PendingType::rename, .src = tmp, .dest = path});
            } else if (object.type == object::type::tree)
                paths.add_pending({.type = PendingType::create_directory, .dest = path});
        }
    }
}

static void clone_remote(Paths& paths, std::string& link, bool clone_versioning, char** error_message) {
    // TODO
}

void repository::clone(Paths& paths, std::string path, bool clone_versioning, char** error_message) {
    if (std::filesystem::is_directory(path))
        clone_local(paths, path, clone_versioning, error_message);
    else
        clone_remote(paths, path, clone_versioning, error_message);
}