#include <core.hpp>

std::vector<std::string> version::status(std::filesystem::path lvc) {
    std::string workspace_name        = io::content(lvc / NAME_CURRENT, 0);
    std::filesystem::path workspace   = workspace_path(lvc / NAME_WORKSPACE, workspace_name);
    std::filesystem::path working_dir = lvc.parent_path();

    std::string version = io::content_first_line(workspace);
    std::vector<std::string> status = io::content_lines(lvc / NAME_PREPARE, 0);

    std::vector<Object> objects = version::all_objects(lvc / NAME_OBJECT, working_dir, version);
    
    std::unordered_set<std::string> unmodified;
    unmodified.reserve(objects.size());
    char id[65];
    for (Object& object : objects) {
        std::filesystem::path path = working_dir / object.path;
        if (!std::filesystem::exists(path))
            continue;
        std::string buffer = io::content(path, 0);
        sha256(buffer.data(), buffer.size(), id);
        if (object.id != id)
            continue;
        unmodified.emplace(std::move(object.path));
    }

    return status;
}

std::vector<std::string> version::status_all(std::filesystem::path lvc) {
    return io::content_lines(lvc / NAME_PREPARE, 0);
}