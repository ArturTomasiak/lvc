#include "core.hpp"

struct PendingNested {
    LvcVersion** version;
    std::string  workspace;
};

static LvcVersion* history(
    std::unordered_set<std::string>& saved_versions, Paths& paths, const std::string& workspace_name,
    char** error_message) {
    const std::filesystem::path workspace = paths.workspace / workspace_name;
    if (!std::filesystem::exists(workspace))
        return {};

    std::vector<PendingNested> pending;
    std::vector<std::string>   buffer = io::content_lines(workspace, 1, 1, error_message);
    size_t                     alloc  = 0;

    for (std::string& line : buffer) {
        if (!line.empty() && line.back() == '\r')
            line.pop_back();
        alloc++;
    }

    LvcVersion* version = (LvcVersion*)malloc(sizeof(LvcVersion));
    version->entry      = (LvcVersionEntry*)malloc(alloc * sizeof(LvcVersionEntry));
    version->workspace  = (char*)malloc(workspace_name.size() + 1);
    version->len        = 0;
    memcpy(version->workspace, workspace_name.data(), workspace_name.size() + 1);

    for (std::string& line : buffer) {
        if (saved_versions.contains(line))
            break;

        std::vector<std::string> version_content = io::content_lines(paths.object / line, 1, 0, error_message);

        if (version_content.size() <= VERSION_AUTHOR || line.size() != 64) {
            continue;
        }

        LvcVersionEntry* entry = &version->entry[version->len++];
        entry->nested          = nullptr;
        memcpy(entry->id, line.c_str(), 65);

        size_t desc_size = version_content[VERSION_MESSAGE].size();
        entry->desc      = (char*)malloc(desc_size + 1);
        CHECKALLOC(entry->desc, error_message, version);
        memcpy(entry->desc, version_content[VERSION_MESSAGE].c_str(), desc_size + 1);

        saved_versions.insert(line);

        if (version_content.size() > VERSION_WORKSPACE)
            pending.push_back({&entry->nested, version_content[VERSION_WORKSPACE]});
    }
    for (PendingNested& entry : pending)
        (*entry.version) = history(saved_versions, paths, entry.workspace, error_message);
    return version;
}

LvcVersion* version::history(Paths& paths, const std::string workspace_name, char** error_message) {
    std::unordered_set<std::string> saved_versions;
    saved_versions.reserve(PREALLOCATE);
    return history(saved_versions, paths, workspace_name, error_message);
}

void version::history_free(LvcVersion* version) {
    for (size_t i = 0; i < version->len; i++) {
        LvcVersionEntry* entry = &version->entry[i];
        free(entry->desc);
        if (entry->nested)
            history_free(entry->nested);
    }
    free(version->entry);
    free(version);
}