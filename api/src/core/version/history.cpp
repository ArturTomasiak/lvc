#include "core.hpp"

LvcVersion* version::history(const std::filesystem::path& lvc, const std::string& workspace_name) {
    const std::filesystem::path workspace_dir = lvc / NAME_WORKSPACE;
    const std::filesystem::path object_dir    = lvc / NAME_OBJECT;
    std::filesystem::path workspace = workspace_path(workspace_dir, workspace_name);
    if (!std::filesystem::exists(workspace))
        return {};
    std::ifstream file(workspace, std::ios::binary);
    if (!file.is_open())
        return {};

    std::string line;
    LvcVersion* first_version = nullptr;
    LvcVersion* version       = nullptr; 
    bool first = 1;
    while (std::getline(file, line)) {
        if (!line.empty() && line.back() == '\r')
            line.pop_back();
        
        std::vector<std::string> version_content = io::content_lines(object_dir / line, 1);

        if (version_content.size() <= VERSION_AUTHOR || line.size() != 64) {
            continue;
        }

        if (first) {
            version = (LvcVersion*)malloc(sizeof(LvcVersion));
            first_version = version;
            first = 0;
        } else {
            version->previous = (LvcVersion*)malloc(sizeof(LvcVersion));
            version = version->previous;
        }

        version->previous        = nullptr;
        version->nested_versions = nullptr;
        
        memcpy(version->id, line.c_str(), 65);

        size_t desc_size = version_content[VERSION_MESSAGE].size();
        version->description = (char*)malloc(desc_size + 1);
        memcpy(version->description, version_content[VERSION_MESSAGE].c_str(), desc_size + 1);
        if (version_content.size() -1 >= VERSION_WORKSPACE)
            version->nested_versions = version::history(lvc, version_content[VERSION_WORKSPACE]);
    }

    return first_version;
}

void version::history_free(LvcVersion* version) {
    while (version) {
        LvcVersion* tmp = version; 
        version = version->previous;
        free(tmp->description);
        if (tmp->nested_versions)
            version::history_free(tmp->nested_versions);
        free(tmp);
    }
}