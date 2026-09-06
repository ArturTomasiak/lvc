#include <core.hpp>

static void category_exists_error(std::string category_name, char** error_message) {
    error_message_creator("Category " + category_name + " does not exist", error_message);
}

void workspace::move_categories(
    Paths& paths, const char* workspace_name, const char* previous_category, const char* category,
    char** error_message) {
    if (charpcmp(previous_category, NAME_INACTIVE) || charpcmp(category, NAME_INACTIVE))
        error_message_creator("Moving to and from inactive is forbidden", error_message);
    else if (!category::exists(paths, previous_category))
        category_exists_error(previous_category, error_message);
    else if (!category::exists(paths, category))
        category_exists_error(category, error_message);
    else if (!workspace::exists(paths, workspace_name, error_message))
        error_message_creator("Workspace does not exist", error_message);

    std::filesystem::path oldp = paths.workspace / previous_category / workspace_name;

    if (!std::filesystem::is_regular_file(oldp))
        error_message_creator("Workspace does not belong to that category", error_message);
    if (*error_message)
        return;

    std::filesystem::path newp = paths.workspace / category / workspace_name;
    std::error_code       ec;
    std::filesystem::rename(oldp, newp, ec);
    if (ec)
        error_message_creator("Failed to move categories", error_message);
}