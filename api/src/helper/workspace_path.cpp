#include <helper.hpp>

std::filesystem::path workspace_path(const std::filesystem::path& workspace_dir, const std::string& workspace_name) {
    std::filesystem::path result;

    std::error_code                               error;
    const std::filesystem::directory_options      options = std::filesystem::directory_options::skip_permission_denied;
    std::filesystem::recursive_directory_iterator iterator(workspace_dir, options, error);
    const std::filesystem::recursive_directory_iterator end;

    if (error)
        return result;

    while (iterator != end) {
        error.clear();
        if (!error && iterator->is_regular_file(error)) {
            result = iterator->path();
            return result;
        }
        iterator.increment(error);
    }

    return result;
}