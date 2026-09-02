#include "core.hpp"

static LvcVersion* history(
    LvcVersion*& allocated, const std::filesystem::path& object_dir, const std::filesystem::path& workspace_dir, const std::string& workspace_name,
    size_t depth, size_t length, size_t& index, char** error_message) {
    std::filesystem::path workspace = workspace_path(workspace_dir, workspace_name);
    if (!std::filesystem::exists(workspace)) return {};
    std::ifstream file(workspace, std::ios::binary);
    if (!file.is_open()) return {};

    std::string line;
    LvcVersion* first_version = nullptr;
    LvcVersion* version       = nullptr;
    bool        first         = 1;
    while (std::getline(file, line)) {
        if (index == length) return first_version;

        if (!line.empty() && line.back() == '\r') line.pop_back();

        std::vector<std::string> version_content = io::content_lines(object_dir / line, 1, error_message);

        if (version_content.size() <= VERSION_AUTHOR || line.size() != 64) {
            continue;
        }

        if (first) {
            version       = &allocated[index++];
            first_version = version;
            first         = 0;
        } else {
            version->previous = &allocated[index++];
            version           = version->previous;
        }

        version->previous        = nullptr;
        version->nested_versions = nullptr;

        memcpy(version->id, line.c_str(), 65);

        size_t desc_size     = version_content[VERSION_MESSAGE].size();
        version->description = (char*)malloc(desc_size + 1);
        memcpy(version->description, version_content[VERSION_MESSAGE].c_str(), desc_size + 1);
        if (depth && version_content.size() - 1 >= VERSION_WORKSPACE)
            version->nested_versions =
                history(allocated, object_dir, workspace_dir, version_content[VERSION_WORKSPACE], depth - 1, length, index, error_message);
    }

    return first_version;
}

LvcVersion* version::history(
    const std::filesystem::path object_dir, const std::filesystem::path workspace_dir, const std::string workspace_name, size_t depth, size_t length,
    char** error_message) {
    LvcVersion* allocated = (LvcVersion*)malloc(length * sizeof(LvcVersion));
    size_t      index     = 0;
    return history(allocated, object_dir, workspace_dir, workspace_name, depth, length, index, error_message);
}

static LvcVersion* history_all(
    LvcVersion*& allocated, const std::filesystem::path& object_dir, const std::filesystem::path& workspace_dir, const std::string& workspace_name,
    size_t& allocated_size, size_t& index, char** error_message) {
    std::filesystem::path workspace = workspace_path(workspace_dir, workspace_name);
    if (!std::filesystem::exists(workspace)) return {};
    std::ifstream file(workspace, std::ios::binary);
    if (!file.is_open()) return {};

    std::string line;
    LvcVersion* first_version = nullptr;
    LvcVersion* version       = nullptr;
    bool        first         = 1;
    while (std::getline(file, line)) {
        if (index == allocated_size) {
            allocated_size *= 2;
            LvcVersion* tmp = (LvcVersion*)realloc(allocated, allocated_size * sizeof(LvcVersion));
            if (!tmp) return first_version;
            allocated = tmp;
        }

        if (!line.empty() && line.back() == '\r') line.pop_back();

        std::vector<std::string> version_content = io::content_lines(object_dir / line, 1, error_message);

        if (version_content.size() <= VERSION_AUTHOR || line.size() != 64) {
            continue;
        }

        if (first) {
            version       = &allocated[index++];
            first_version = version;
            first         = 0;
        } else {
            version->previous = &allocated[index++];
            version           = version->previous;
        }

        version->previous        = nullptr;
        version->nested_versions = nullptr;

        memcpy(version->id, line.c_str(), 65);

        size_t desc_size     = version_content[VERSION_MESSAGE].size();
        version->description = (char*)malloc(desc_size + 1);
        memcpy(version->description, version_content[VERSION_MESSAGE].c_str(), desc_size + 1);
        if (version_content.size() >= VERSION_WORKSPACE)
            version->nested_versions =
                history_all(allocated, object_dir, workspace_dir, version_content[VERSION_WORKSPACE], allocated_size, index, error_message);
    }

    return first_version;
}

LvcVersion* version::history_all(
    const std::filesystem::path object_dir, const std::filesystem::path workspace_dir, const std::string workspace_name, char** error_message) {
    size_t      allocated_size = 50;
    size_t      index          = 0;
    LvcVersion* allocated      = (LvcVersion*)malloc(allocated_size * sizeof(LvcVersion));
    return history_all(allocated, object_dir, workspace_dir, workspace_name, allocated_size, index, error_message);
}

void history_free_descriptions(LvcVersion* version) {
    while (version) {
        free(version->description);
        if (version->nested_versions) history_free_descriptions(version->nested_versions);
        version = version->previous;
    }
}

void version::history_free(LvcVersion* version) {
    if (!version) return;
    history_free_descriptions(version);
    free(version);
}