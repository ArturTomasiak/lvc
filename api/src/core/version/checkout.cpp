#include <core.hpp>

std::vector<std::string> version::checkout(Paths& paths, char** error_message) {
    const std::string              branch_name    = io::content(paths.current, 0, 0, error_message);
    const std::filesystem::path    branch         = paths.branch / branch_name;
    const std::vector<std::string> branch_content = io::content_lines(branch, 1, 1, error_message);
    const std::string              version        = branch_content.empty() ? "" : branch_content[0];

    std::unordered_map<std::filesystem::path, object::info> version_map = version::map_all_objects(paths, version, {}, error_message);
    std::vector<object::info>                               working_dir = branch::all_objects(paths, error_message);
    const std::vector<std::string>                          ignore = paths.resolve(io::content_lines(paths.ignore, 0, 0, error_message), 1);

    std::unordered_set<std::filesystem::path> ignore_entries;
    ignore_entries.reserve(ignore.size());
    for (const std::string& entry : ignore)
        ignore_entries.insert(std::move(entry));

    std::erase_if(working_dir, [&ignore_entries](const object::info& entry) { return ignore_entries.contains(entry.path); });

    std::vector<std::string> result;
    result.reserve(working_dir.size());
    for (const object::info& object : working_dir) {
        if (object.type == object::type::tree)
            continue;
        const auto it = version_map.find(object.path);
        if (it == version_map.end()) {
            result.push_back(object.path.generic_string());
            continue;
        }
        const object::info* v = &it->second;
        if (object.id != v->id)
            result.push_back(object.path.generic_string());
        version_map.erase(it);
    }

    for (const auto& [path, info] : version_map)
        if (info.type == object::type::blob && !ignore_entries.contains(path))
            result.push_back(path.generic_string());

    return result;
}