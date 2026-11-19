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
    list->category = static_cast<LvcCategory*>(realloc(list->category, (list->len + 1) * sizeof(LvcCategory)));
    CHECKALLOC(list->category, error_message, {});
    LvcCategory* out    = &list->category[list->len];
    out->branches_len   = 0;
    out->branches_alloc = 8;
    out->branches       = static_cast<char**>(malloc(out->branches_alloc * sizeof(char*)));
    out->name           = static_cast<char*>(malloc(name.size() + 1));
    CHECKALLOC(out->branches, error_message, {});
    CHECKALLOC(out->name, error_message, {});
    memcpy(out->name, name.data(), name.size() + 1);
    list->len++;
    return out;
}

void add_branch(LvcCategory* category, const std::string& name, char** error_message) {
    if (category->branches_len == category->branches_alloc) {
        category->branches_alloc *= 2;
        category->branches = static_cast<char**>(realloc(category->branches, category->branches_alloc * sizeof(char*)));
        CHECKALLOC(category->branches, error_message);
    }
    char* branch = static_cast<char*>(malloc(name.size() + 1));
    CHECKALLOC(branch, error_message);
    memcpy(branch, name.data(), name.size() + 1);
    category->branches[category->branches_len] = branch;
    category->branches_len++;
}

LvcCategoryList* category::list(Paths& paths, char** error_message) {
    LvcCategoryList* out       = static_cast<LvcCategoryList*>(malloc(sizeof(LvcCategoryList)));
    out->category              = nullptr;
    out->len                   = 0;
    bool uncategorized_created = 0;

    for (const auto& entry : std::filesystem::directory_iterator(paths.branch)) {
        if (!entry.is_regular_file())
            continue;
        const std::filesystem::path path = entry.path();
        const std::string           name = path.filename();
        LvcCategory*                current;
        const std::string           buffer   = io::content(path, 1, 0, error_message);
        const std::string           category = category::get(buffer);
        if (category.empty()) {
            if (uncategorized_created == 0) {
                current               = add_category(out, LVC_CATEGORY_UNCATEGORIZED, error_message);
                uncategorized_created = 1;
            } else
                current = find_category(out, LVC_CATEGORY_UNCATEGORIZED);
        } else {
            current = find_category(out, category);
            if (current == nullptr) {
                current = add_category(out, category, error_message);
                CHECKERROR(error_message, out);
            }
        }
        add_branch(current, name, error_message);
        CHECKERROR(error_message, out);
    }

    return out;
}

void category::free_category_list(LvcCategoryList* list) {
    for (size_t i = 0; i < list->len; ++i) {
        LvcCategory& category = list->category[i];

        for (size_t j = 0; j < category.branches_len; ++j)
            free(category.branches[j]);

        free(category.branches);
        free(category.name);
    }

    free(list->category);
    list->category = nullptr;
    list->len      = 0;
}