#include "core.hpp"

std::vector<ObjectDiff> object::find_difference(const std::vector<Object>& src, const std::vector<Object>& dest) {
    std::unordered_map<std::filesystem::path, const Object*> dest_by_path;
    std::unordered_map<std::string, const Object*> dest_by_id;
    dest_by_path.reserve(dest.size());
    dest_by_id.reserve(dest.size());
    for (const Object& object : dest) {
        dest_by_path.emplace(object.path, &object);
        dest_by_id.emplace(object.id, &object);
    }

    std::vector<ObjectDiff> diff;
    diff.reserve(PREALLOCATE);

    for (const Object& object : src) {
        if (auto it = dest_by_path.find(object.path); it != dest_by_path.end()) {
            const Object& found = *it->second;

            if (object.id != found.id)
                diff.push_back({object, found});
        }

        if (auto it = dest_by_id.find(object.id); it != dest_by_id.end()) {
            const Object& found = *it->second;

            if (object.path != found.path)
                diff.push_back({object, found});
        }
    }

    return diff;
}