#include "core.hpp"

static void clone_local(Paths& paths, std::filesystem::path path, bool clone_versioning, char** error_message) {
    if (!std::filesystem::is_directory(path / ".lvc")) {
        error_message_creator("Clone has no .lvc folder", error_message);
        return;
    }

    std::error_code ec;
    try {
        std::filesystem::recursive_directory_iterator       iterator(path);
        const std::filesystem::recursive_directory_iterator end;
        for (; iterator != end; iterator++) {
            const std::filesystem::directory_entry& entry    = *iterator;
            const std::filesystem::path             relative = entry.path().lexically_relative(path);

            if (!clone_versioning && iterator.depth() == 0 && relative.filename() == ".lvc") {
                iterator.disable_recursion_pending();
                continue;
            }

            if (std::filesystem::is_regular_file(entry.path())) {
                const std::filesystem::path dest = paths.root / relative;
                const std::filesystem::path tmp  = paths.add_pending_rename(dest);
                std::filesystem::copy(entry.path(), tmp, ec);
                if (ec) {
                    error_message_creator("Failed to copy file " + entry.path().filename().string(), error_message);
                    return;
                }
            } else if (std::filesystem::is_directory(entry.path())) {
                std::filesystem::create_directories(paths.tmp / relative);
                paths.pending_directory.emplace_back(std::move(relative));
            }
        }
    } catch (const std::filesystem::filesystem_error& error) {
        error_message_creator("Directory iteration failure", error_message);
        return;
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