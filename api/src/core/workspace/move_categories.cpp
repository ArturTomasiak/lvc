#include <core.hpp>

LvcError workspace::move_categories(std::filesystem::path workspace_dir, const char* workspace_name, const char* previous_category, const char* category) {
    if (charpcmp(previous_category, NAME_INACTIVE) || charpcmp(category, NAME_INACTIVE))
        return MOVE_INACTIVE;
    if (!category::exists(workspace_dir, previous_category) || !category::exists(workspace_dir, category))
        return CATEGORY_NOT_EXISTS;
    std::filesystem::path oldp = workspace_dir / previous_category / workspace_name;
    std::filesystem::path newp = workspace_dir / category / workspace_name;
    std::error_code ec;
    std::filesystem::rename(oldp, newp, ec);
    return ec ? WORKSPACE_MOVE_CATEGORIES : SUCCESS;
}