#include "core.hpp"

static LvcVersion* create_version(const std::string& workspace_name, char** error_message) {
    LvcVersion* out = reinterpret_cast<LvcVersion*>(malloc(sizeof(LvcVersion)));
    CHECKALLOC(out, error_message, {});

    out->workspace = reinterpret_cast<char*>(malloc(workspace_name.size() + 1));
    CHECKALLOC(out->workspace, error_message, {});
    memcpy(out->workspace, workspace_name.c_str(), workspace_name.size() + 1);

    out->len   = 0;
    out->alloc = 10;
    out->entry = reinterpret_cast<LvcVersionEntry*>(malloc(sizeof(LvcVersionEntry) * out->alloc));
    CHECKALLOC(out->entry, error_message, {});

    return out;
}

static void add_entry(LvcVersion* version, const std::string& id, const std::string& desc, char** error_message) {
    if (version->len == version->alloc) {
        version->alloc *= 2;
        version->entry =
            reinterpret_cast<LvcVersionEntry*>(realloc(version->entry, sizeof(LvcVersionEntry) * version->alloc));
        CHECKALLOC(version->entry, error_message);
    }
    LvcVersionEntry* entry = &version->entry[version->len++];
    entry->desc            = reinterpret_cast<char*>(malloc(desc.size() + 1));
    CHECKALLOC(entry->desc, error_message);
    memcpy(entry->id, id.c_str(), id.size() + 1);
    memcpy(entry->desc, desc.c_str(), desc.size() + 1);
    entry->nested = nullptr;
}

LvcVersion* version::history(
    Paths& paths, const std::string& workspace_name, char** error_message, std::string from, std::string to) {
    const std::filesystem::path workspace = paths.workspace / workspace_name;
    if (!std::filesystem::exists(workspace))
        return {};

    LvcVersion* out = create_version(workspace_name, error_message);

    std::vector<std::string> buffer = io::content_lines(workspace, 1, 1, error_message);

    if (!from.empty())
        while (!buffer.empty() && buffer[0] != from)
            buffer.erase(buffer.begin());

    for (const std::string& version : buffer) {
        if (!to.empty() && version == to)
            break;

        std::vector<std::string> version_content   = io::content_lines(paths.object / version, 1, 0, error_message);
        const std::string&       version_workspace = version_content[VERSION_WORKSPACE];
        if (version_content.size() > VERSION_WORKSPACE_TO)
            version::history(
                paths, version_content[VERSION_WORKSPACE], error_message, version_content[VERSION_WORKSPACE_FROM],
                version_content[VERSION_WORKSPACE_TO]);

        add_entry(out, version, version_content[VERSION_MESSAGE], error_message);
    }

    return out;
}

void version::history_free(LvcVersion* version) {
    for (size_t i = 0; i < version->len; i++) {
        LvcVersionEntry* entry = &version->entry[i];
        if (entry->desc)
            free(entry->desc);
        if (entry->nested)
            history_free(entry->nested);
    }
    if (version)
        free(version);
}