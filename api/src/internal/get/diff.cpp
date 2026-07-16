#include <internal.hpp>

std::vector<std::string> get::diff(std::filesystem::path lvc) {
    std::filesystem::path branch = lvc / NAME_WORKSPACE / get::file_content(lvc / NAME_CURRENT);
    std::vector<std::string> result;
    result.reserve(PREALLOCATE);
    std::string version = get::latest_version(branch); 
    std::unordered_set<std::string> file_hashes = get::objects_in_version(lvc, version);
    return result;
}