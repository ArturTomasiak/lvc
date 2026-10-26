#include <core.hpp>

LvcCategory* find_category(LvcCategoryList* list, const std::string& name) {
    for (size_t i = 0; i < list->len; i++) {
        LvcCategory* category = &list->category[i];
        if (!strcmp(category->name, name.c_str()))
            return category;
    }
    return nullptr;
}

LvcCategory* add_category(LvcCategoryList* list, const std::string& name, char** error_message) {
    list->category = (LvcCategory*)realloc(list->category, (list->len + 1) * sizeof(LvcCategory));
    CHECKALLOC(list->category, error_message, {});
    LvcCategory* out      = &list->category[list->len];
    out->workspaces_len   = 0;
    out->workspaces_alloc = 8;
    out->workspaces       = (char**)malloc(out->workspaces_alloc * sizeof(char*));
    out->name             = (char*)malloc(name.size() + 1);
    CHECKALLOC(out->workspaces, error_message, {});
    CHECKALLOC(out->name, error_message, {});
    memcpy(out->name, name.data(), name.size() + 1);
    list->len++;
    return out;
}

void add_workspace(LvcCategory* category, const std::string& name, char** error_message) {
    if (category->workspaces_len == category->workspaces_alloc) {
        category->workspaces_alloc *= 2;
        category->workspaces = (char**)realloc(category->workspaces, category->workspaces_alloc * sizeof(char*));
        CHECKALLOC(category->workspaces, error_message);
    }
    char* workspace = (char*)malloc(name.size() + 1);
    CHECKALLOC(workspace, error_message);
    memcpy(workspace, name.data(), name.size() + 1);
    category->workspaces[category->workspaces_len] = workspace;
    category->workspaces_len++;
}

LvcCategoryList* category::list(Paths& paths, char** error_message) {
    LvcCategoryList* out = (LvcCategoryList*)malloc(sizeof(LvcCategoryList));
    out->category = nullptr;
    out->len      = 0;
    bool uncategorized_created = 0;

    for (const auto& entry : std::filesystem::directory_iterator(paths.workspace)) {
        if (!entry.is_regular_file())
            continue;
        const std::filesystem::path path = entry.path();
        const std::string           name = path.filename();
        LvcCategory*                current;
        std::string                 buffer = io::content(path, 1, error_message);
        size_t                      pos    = buffer.find(' ');
        buffer.erase(0, pos + 1);
        pos = buffer.find(' ');
        buffer.erase(0, pos + 1);
        if (buffer[0] == '\n') {
            if (uncategorized_created == 0) {
                current = add_category(out, LVC_CATEGORY_UNCATEGORIZED, error_message);
                uncategorized_created = 1;
            }
            else 
                current = find_category(out, LVC_CATEGORY_UNCATEGORIZED);
        }
        else {
            pos                    = buffer.find('\n');
            const std::string name = buffer.substr(0, pos);
            current                = find_category(out, name);
            if (current == nullptr) {
                current = add_category(out, name, error_message);
                if (*error_message != 0)
                    return out;
            }
        }
        add_workspace(current, name, error_message);
        if (*error_message != 0)
            return out;
    }

    return out;
}

void category::free_category_list(LvcCategoryList* list) {
    for (size_t i = 0; i < list->len; ++i) {
        LvcCategory& category = list->category[i];

        for (size_t j = 0; j < category.workspaces_len; ++j)
            free(category.workspaces[j]);

        free(category.workspaces);
        free(category.name);
    }

    free(list->category);
    list->category = nullptr;
    list->len      = 0;
}