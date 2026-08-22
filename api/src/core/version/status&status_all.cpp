#include <core.hpp>

std::vector<std::string> version::status(std::filesystem::path lvc) {
    std::string workspace_name        = io::content(lvc / NAME_CURRENT, 0);
    std::filesystem::path workspace   = workspace_path(lvc / NAME_WORKSPACE, workspace_name);
    std::filesystem::path working_dir = lvc.parent_path();

    std::string version = io::content_first_line(workspace);
    std::vector<std::string> status = io::content_lines(lvc / NAME_PREPARE, 0);

    std::vector<Object> objects = version::all_objects(lvc / NAME_OBJECT, version, {});
    
    std::unordered_set<std::string> unmodified;
    unmodified.reserve(objects.size());
    char id[65];
    for (Object& object : objects) { 
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
    std::erase_if(status, [&](const std::string& path) {
        return unmodified.contains(path);
    });

    return status;
}

std::vector<std::string> version::status_all(std::filesystem::path lvc) {
    return io::content_lines(lvc / NAME_PREPARE, 0);
}