#include "core.hpp"

static void validate_local(std::filesystem::path& path, char** error_message) {
    if (!std::filesystem::is_directory(path / ".lvc"))
        error_message_creator("Clone has no .lvc folder", error_message);
}

static void clone_local(Paths& paths, std::filesystem::path path, bool clone_versioning, char** error_message) {
    Paths             their_paths((path / ".lvc").c_str(), error_message);
    const std::string default_workspace_name = io::content(their_paths.lvc / filename_default, 0, 0, error_message);
    const std::vector<std::string> default_workspace =
        io::content_lines(their_paths.workspace / default_workspace_name, 1, 1, error_message);
    const std::string version = default_workspace.empty() ? std::string{} : default_workspace[0];
    if (!std::filesystem::is_directory(their_paths.lvc)) {
        error_message_creator("Clone has no .lvc folder", error_message);
        return;
    }

    if (clone_versioning) {
        std::error_code ec;
        std::filesystem::copy(
            path / ".lvc", paths.lvc,
            std::filesystem::copy_options::recursive | std::filesystem::copy_options::overwrite_existing, ec);
        if (ec) {
            error_message_creator("Failed to copy the lvc folder", error_message);
            return;
        }
    }

    if (!version.empty()) {
        const std::vector<object::info> objects = version::all_objects(their_paths.object, version, {}, error_message);
        for (const object::info& object : objects) {
            const std::filesystem::path path   = paths.root / object.path;
            const std::filesystem::path parent = path.parent_path();
            if (!std::filesystem::is_directory(parent))
                std::filesystem::create_directories(path.parent_path());
            if (object.type == object::type::blob) {
                std::string buffer = io::content(their_paths.object / object.id, 1, 1, error_message);
                io::file(
                    paths.root / path, std::ios::binary | std::ios::trunc, buffer.data(), buffer.size(), 0,
                    error_message);
            } else if (object.type == object::type::tree)
                std::filesystem::create_directory(path);
        }
    }

    io::file(
        paths.current, std::ios::binary | std::ios::trunc, default_workspace_name.c_str(),
        default_workspace_name.size(), 0, error_message);
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