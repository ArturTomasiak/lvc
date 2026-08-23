#include "core.hpp"

static void validate_local(std::filesystem::path path, char** error_message) {
    std::filesystem::path lvc = path / ".lvc";
    if (!std::filesystem::is_directory(lvc))
        error_message_creator("Clone has no .lvc folder", error_message);
}

static void clone_local(std::filesystem::path& working_dir, std::filesystem::path path, bool& clone_versioning, char** error_message) {
    validate_local(path, error_message);
    if (error_message) return;
    std::error_code ec;

try {
    std::filesystem::directory_iterator iterator(path);
    for (const std::filesystem::directory_entry& entry : iterator) {
        std::string name = entry.path().filename();
        if (!clone_versioning && name == ".lvc")
            continue;

        std::filesystem::copy(
            entry.path(), 
            working_dir / entry.path().filename(), 
            std::filesystem::copy_options::recursive | std::filesystem::copy_options::overwrite_existing,
            ec
        );
        if (ec) {
            error_message_creator_path("Failed to copy file", entry.path().filename(), error_message);
            return;
        }
    }
}
catch(const std::filesystem::filesystem_error& error) {
    error_message_creator("Directory iteration failure", error_message);
        return;
}
}

static void clone_remote(std::filesystem::path& working_dir, std::string& link, bool& clone_versioning, char** error_message) {
   // TODO
}

void repository::clone(std::filesystem::path working_dir, std::string path, bool clone_versioning, char** error_message) {
    if (std::filesystem::is_directory(path))
        clone_local(working_dir, path, clone_versioning, error_message);
    else 
        clone_remote(working_dir, path, clone_versioning, error_message);
}