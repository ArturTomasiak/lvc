#include <app.hpp>

bool lvc::list(int argc, char* argv[]) {
    std::filesystem::path lvc;
     bool all = true;
    if (!helper::find_repository(lvc))
        return 0;

    if (argc == 3) {
        all = false;
        if (!lvc_category_exists(lvc.c_str(), argv[2], &error_message)) {
            helper::error("Invalid category: " + std::string(argv[2]));
            return false;
        }
    }

    LvcCategoryList* list = lvc_category_list(lvc.c_str(), &error_message);

    if (error_message) {
        if (list != nullptr)
            lvc_category_list_free(list);
        return false;
    }

    for (size_t i = 0; i < list->len; i++) {
        LvcCategory* category = &list->category[i];

        if (all)
            std::cout << category->name << '\n';
        else if (strcmp(argv[2], category->name))
            continue;

        for (size_t i = 0; i < category->workspaces_len; i++) {
            std::cout << category->name << "/" << category->workspaces[i] << '\n';
        }
    }

    lvc_category_list_free(list);

    return true;
}