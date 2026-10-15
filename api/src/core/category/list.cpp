#include <core.hpp>

LvcCategoryList category::list(Paths& paths, char** error_message) {
    LvcCategoryList out{};

    if (!std::filesystem::exists(paths.workspace) || !std::filesystem::is_directory(paths.workspace))
        return out;

    for (const auto& entry : std::filesystem::directory_iterator(paths.workspace)) {
        if (!entry.is_directory())
            continue;

        const std::filesystem::path path     = entry.path();
        const std::string           path_str = path.string();

        if (path == paths.local)
            continue;

        std::vector<std::string> files;
        files.reserve(PREALLOCATE);

        for (const auto& file : std::filesystem::directory_iterator(path)) {
            if (!file.is_regular_file())
                continue;
            files.push_back(file.path().string());
        }

        out.category = (LvcCategory*)realloc(out.category, (out.len + 1) * sizeof(LvcCategory));

        if (out.category == NULL) {
            error_message_creator("Memory allocation failed", error_message);
            return out;
        }

        LvcCategory* category    = &out.category[out.len];
        category->workspaces_len = files.size();
        category->workspaces     = (char**)malloc(category->workspaces_len * sizeof(char*));
        category->name           = (char*)malloc(path_str.size() + 1);

        if (category->workspaces == NULL || category->name == NULL) {
            error_message_creator("Memory allocation failed", error_message);
            return out;
        }

        memcpy(category->name, path.c_str(), path_str.size() + 1);
        for (size_t i = 0; i < files.size(); i++) {
            std::string& file       = files[i];
            category->workspaces[i] = (char*)malloc(file.size() + 1);

            if (category->workspaces == NULL) {
                error_message_creator("Memory allocation failed", error_message);
                return out;
            }

            char* workspace = category->workspaces[i];
            memcpy(workspace, file.c_str(), file.size() + 1);
        }
        out.len++;
    }

    return out;
}

void category::free_category_list(LvcCategoryList& list) {
    for (size_t i = 0; i < list.len; ++i) {
        LvcCategory& category = list.category[i];

        for (size_t j = 0; j < category.workspaces_len; ++j)
            free(category.workspaces[j]);

        free(category.workspaces);
        free(category.name);
    }

    free(list.category);
    list.category = nullptr;
    list.len      = 0;
}