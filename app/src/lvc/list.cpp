#include <app.hpp>

bool lvc::list(int argc, char* argv[]) {
    std::filesystem::path lvc;
    if (!helper::find_repository(lvc)) {
        helper::error("Couldn't find LVC repository");
        return 0;
    }
    LvcCategoryList* list = lvc_category_list(lvc.c_str(), &error_message);

    if (error_message) {
        if (list != nullptr)
            lvc_category_list_free(list);
        return false;
    }

    for (size_t i = 0; i < list->len; i++) {
        LvcCategory* category = &list->category[i];
        if (list->len != 1)
            std::cout << category->name << '\n';
        for (size_t i = 0; i < category->workspaces_len; i++) {
            std::cout << "    " << category->workspaces[i] << '\n';
        }
    }

    lvc_category_list_free(list);
    return true;
}