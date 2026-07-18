#include <internal.hpp>

std::vector<std::string> version::diff(std::filesystem::path lvc) {
    std::filesystem::path branch = lvc / NAME_WORKSPACE / file::content(lvc / NAME_CURRENT, 0);
    std::string version = version::latest(branch); 

    std::unordered_set<std::string> versioned_id = version::objects(lvc / NAME_OBJECT, version);
    std::vector<std::string> working_id = sha256::working_directory(lvc.parent_path());

    std::vector<std::string> result;
    result.reserve(working_id.size());

    for (const std::string& id : working_id)
        if (!versioned_id.contains(id))
            result.emplace_back(id);

    return result;
}