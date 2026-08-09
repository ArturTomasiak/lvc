#include "core.hpp"

static LvcError validate_local(std::filesystem::path path) {
    std::filesystem::path lvc = path / ".lvc";
    if (!std::filesystem::is_directory(lvc))
        return CLONE_NO_LVC;

    return SUCCESS;
}

static LvcError clone_local(std::filesystem::path& working_dir, std::filesystem::path path, bool& clone_versioning) {
    RETURN_ERR(validate_local(path));
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
        if (ec)
            return FILESYSTEM_COPY_ERROR;
    }
}
catch(const std::filesystem::filesystem_error& error) {
    return WORKING_DIR_ITERATION_FAILED;
}
    
    return SUCCESS;
}

static LvcError clone_remote(std::filesystem::path& working_dir, std::string& link, bool& clone_versioning) {
    return SUCCESS;
}

LvcError repository::clone(std::filesystem::path working_dir, std::string path, bool clone_versioning) {
    if (std::filesystem::is_directory(path))
        return clone_local(working_dir, path, clone_versioning);
    else 
        return SUCCESS; //TODO
}