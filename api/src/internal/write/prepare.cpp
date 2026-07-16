#include <internal.hpp>

LvcError write::prepare(std::filesystem::path lvc, std::vector<std::string> input) {
    std::vector<std::string> paths   = get::path_from_input(lvc.parent_path(), input);
    if (paths.empty())
        return PREPARE_NO_INPUT;
    std::vector<std::string> current = get::file_content_lines(lvc / "prepare");
    std::unordered_set<std::string> existing;
    existing.reserve(current.size() + paths.size());

    for (const std::string& value : current)
        existing.insert(value);

    for (std::string& path : paths)
        if (existing.insert(path).second)
            current.push_back(std::move(path));

    char* result = strvector_to_charp(current);
    if (!result)
        return MEMORY_ALLOCATION_FAILED;
    write::file(lvc / "prepare", std::ios::binary, result, charplen(result));
    free(result);
    return SUCCESS;
}