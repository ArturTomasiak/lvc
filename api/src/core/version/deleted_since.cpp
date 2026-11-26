#include <core.hpp>

std::vector<std::filesystem::path> version::deleted_since(
    Paths& paths, std::string id, const std::unordered_map<std::filesystem::path, object::info>& version_map, char** error_message) {
    std::vector<std::filesystem::path> out;
    out.reserve(PREALLOCATE);

    try {
        std::filesystem::recursive_directory_iterator iterator(paths.root);
        for (const std::filesystem::directory_entry& entry : iterator) {
            const std::filesystem::path relative = entry.path().lexically_relative(paths.root);
            if (entry.is_directory() && relative == ".lvc") {
                iterator.disable_recursion_pending();
                continue;
            }
            if (!version_map.contains(relative))
                out.push_back(relative);
        }
    } catch (const std::filesystem::filesystem_error& error) {
        error_message_creator("Directory iteration failure", error_message);
        return {};
    }

    return out;
}