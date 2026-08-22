#include <core.hpp>

static void get_unmodified(const std::filesystem::path& object_folder, const std::filesystem::path& working_dir, std::unordered_set<std::string>& unmodified, std::unordered_set<std::string>& status, const std::string tree_id, const std::filesystem::path current) {
    char id[65];
    std::vector<std::string> content = io::content_lines(object_folder / tree_id, 1);
    for (size_t i = 1; i < content.size(); i++) {
        Object object;
        const std::string& line = content[i];

        size_t position = line.find(' ');
        std::string type  = line.substr(0, position);

        object.type = type == TYPE_TREE ? TREE : BLOB;

        object.id        = line.substr(position + 1);
        position         = object.id.find(' ');
        std::string name = object.id.substr(position + 1);
        object.id        = object.id.substr(0, position);
        object.path      = current / name;

        if (status.contains(object.path)) {
            std::filesystem::path path = working_dir / object.path;
            if (std::filesystem::is_regular_file(path)) {
                std::string buffer = io::content(path, 0);
                insert_pattern(buffer, TYPE_BLOB);
                sha256(buffer.data(), buffer.size(), id);
                if (object.id != id)
                    continue;
            }
            else if (!std::filesystem::is_directory(path))
                continue;
            unmodified.emplace(std::move(object.path));
        }

        if (type == TYPE_TREE)
            get_unmodified(object_folder, working_dir, unmodified, status, object.id, object.path);
    }
}

std::vector<std::string> version::status(std::filesystem::path& lvc, std::string& latest_version) {
    std::filesystem::path working_dir = lvc.parent_path();

    std::vector<std::string> status = io::content_lines(lvc / NAME_PREPARE, 0);
    std::unordered_set<std::string> status_set;
    status_set.reserve(status.size());
    for (const std::string& entry : status)
        status_set.insert(entry);

    std::filesystem::path object_dir = lvc / NAME_OBJECT;
    std::unordered_set<std::string> unmodified;
    unmodified.reserve(status.size());
    std::vector<std::string> version_content = io::content_lines(object_dir / latest_version, 1);
    if (version_content.empty())
        return {};
    get_unmodified(object_dir, working_dir, unmodified, status_set, version_content[VERSION_ROOT_TREE], "");

    std::erase_if(status, [&](const std::string& path) {
        return unmodified.contains(path);
    });

    return status;
}

std::vector<std::string> version::status_all(std::filesystem::path lvc) {
    return io::content_lines(lvc / NAME_PREPARE, 0);
}