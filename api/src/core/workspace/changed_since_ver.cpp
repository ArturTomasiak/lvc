#include "core.hpp"

bool workspace::changed_since_ver(
    Paths& paths, const std::vector<object::info>& objects_version, char** error_message) {
    const std::vector<object::info> working = workspace::all_objects(paths, error_message);
    if (working.size() != objects_version.size())
        return true;

    std::unordered_map<std::filesystem::path, const object::info*> by_path;
    by_path.reserve(working.size());
    for (const object::info& object : working)
        by_path.emplace(object.path, &object);

    for (const object::info& expected : objects_version) {
        const auto it = by_path.find(expected.path);
        if (it == by_path.end())
            return true;

        const object::info& actual = *it->second;

        if (actual.type != expected.type)
            return true;

        if (expected.type == object::type::blob && actual.id != expected.id)
            return true;
    }

    return false;
}