#include <core.hpp>

void workspace::deactivate(std::filesystem::path workspace_dir, const char* workspace_name, char** error_message) {
    std::filesystem::path oldp = workspace_path(workspace_dir, workspace_name);
    std::filesystem::path newp = workspace_dir / NAME_INACTIVE / workspace_name;
    std::error_code       ec;
    std::filesystem::rename(oldp, newp, ec);
    if(ec) error_message_creator("Failed to deactivate category", error_message);
}

void workspace::activate(std::filesystem::path workspace_dir, const char* workspace_name, const char* category_name, char** error_message) {
    // TODO SEARCH FOR VERSION THAT HAS THIS WORKSPACE AS A RESULT OF INSERT AND
    // REMOVE THE WORKSPACE
    if(!category::exists(workspace_dir, category_name)) {
        error_message_creator("Category does not exist", error_message);
        return;
    }
    std::filesystem::path oldp = workspace_dir / NAME_INACTIVE / workspace_name;
    std::filesystem::path newp = workspace_dir / category_name / workspace_name;

    std::error_code ec;
    std::filesystem::rename(oldp, newp, ec);
    if(ec) error_message_creator("Failed to activate category", error_message);
}