#include "core.hpp"

static LvcVersion* create_version(LvcVersion* previous, const std::string& workspace_name, char** error_message) {
    LvcVersion* out = reinterpret_cast<LvcVersion*>(malloc(sizeof(LvcVersion)));
    CHECKALLOC(out, error_message, {});

    out->workspace = reinterpret_cast<char*>(malloc(workspace_name.size() + 1));
    CHECKALLOC(out->workspace, error_message, {});
    memcpy(out->workspace, workspace_name.c_str(), workspace_name.size() + 1);

    out->len   = 0;
    out->alloc = 10;
    out->entry = reinterpret_cast<LvcVersionEntry*>(malloc(sizeof(LvcVersionEntry) * out->alloc));
    CHECKALLOC(out->entry, error_message, {});

    out->previous = previous;
    return out;
}

static void add_entry(LvcVersion* version, const std::string& id, const std::string& desc, char** error_message) {
    if (version->len == version->alloc) {
        version->alloc *= 2;
        version->entry = reinterpret_cast<LvcVersionEntry*>(realloc(version->entry, sizeof(LvcVersionEntry) * version->alloc));
        CHECKALLOC(version->entry, error_message);
    }
    LvcVersionEntry* entry = &version->entry[version->len++];
    entry->desc = reinterpret_cast<char*>(malloc(desc.size() + 1));
    CHECKALLOC(entry->desc, error_message);
    memcpy(entry->id, id.c_str(), id.size() + 1);
    memcpy(entry->desc, desc.c_str(), desc.size() + 1);
    entry->nested = nullptr;
}

LvcVersion* version::history(Paths& paths, const std::string& workspace_name, char** error_message) {
    const std::filesystem::path workspace = paths.workspace / workspace_name;
    if (!std::filesystem::exists(workspace))
        return {};

    LvcVersion* current = create_version(nullptr, workspace_name, error_message);
    
    const std::vector<std::string> buffer = io::content_lines(workspace, 1, 1, error_message);
    for (const std::string& version : buffer) {

        std::vector<std::string> version_content = io::content_lines(paths.object / version, 1, 0, error_message);
        const std::string& version_workspace = version_content[VERSION_WORKSPACE];

        if (current->workspace != version_workspace) {
            if (current->previous != nullptr && current->previous->workspace == version_workspace) {
                current = current->previous;
            } else {
                LvcVersionEntry* entry = &current->entry[current->len - 1];
                current = create_version(current, version_workspace, error_message);
                entry->nested = current;
            }
        }

        add_entry(current, version, version_content[VERSION_MESSAGE], error_message);
    }

    while (current->previous)
        current = current->previous;
    return current;
}

void version::history_free(LvcVersion* version) {

}