#include <internal.hpp>

LvcError version::prepare(std::filesystem::path lvc, std::vector<std::string> input) {
    std::vector<std::string> paths = file::path_from_input(lvc.parent_path(), input);
    if (paths.empty())
        return PREPARE_NO_INPUT;

    std::vector<std::string> current = file::content_lines(lvc / NAME_PREPARE, 0);

    std::unordered_set<std::string> existing;
    existing.reserve(current.size() + paths.size());
    existing.insert(current.begin(), current.end());

    std::erase_if(paths, [&](const std::string& path) {
        return existing.contains(path);
    });

    for (std::string& path : paths) {
        path += '\n';
        file::create(lvc / NAME_PREPARE, std::ios::binary | std::ios::app, path.c_str(), path.size(), 0);
    }

    return SUCCESS;
}