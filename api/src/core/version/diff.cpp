#include <core.hpp>

static std::vector<Object> working_directory_to_objects(std::filesystem::path working_dir) {
    std::vector<Object> out;
    out.reserve(PREALLOCATE);
    for (const std::filesystem::directory_entry& entry : std::filesystem::recursive_directory_iterator(working_dir)) {
        if (!entry.is_regular_file())
            continue;
        Object object;
        object.path = entry.path().lexically_relative(working_dir);
        object.type = BLOB;
        std::string buffer = content(entry.path(), 0);
        char id[65];
        sha256(buffer.data(), buffer.size(), id);
        object.id = id;
        out.push_back(std::move(object));
    }
    return out;
}

std::vector<std::string> version::diff(std::filesystem::path lvc) {
    std::string workspace_name        = content(lvc / NAME_CURRENT, 0);
    std::filesystem::path workspace   = workspace_path(lvc / NAME_WORKSPACE, workspace_name);
    std::filesystem::path working_dir = lvc.parent_path();
    std::string version = version::latest(workspace); 

    std::vector<Object> objects_version     = version::all_objects(lvc / NAME_OBJECT, working_dir, version);
    std::vector<Object> objects_working_dir = working_directory_to_objects(working_dir);

    std::unordered_set<std::string_view> version_ids;
    version_ids.reserve(objects_version.size());

    for (const Object& object : objects_version)
        version_ids.emplace(object.id);

    std::vector<std::string> result;
    result.reserve(objects_working_dir.size());

    for (const Object& object : objects_working_dir)
        if (!version_ids.contains(object.id))
            result.push_back(std::move(object.path));

    return result;
}