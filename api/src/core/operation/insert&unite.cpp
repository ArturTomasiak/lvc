#include <core.hpp>

static constexpr std::string inactive_error_message = " is inactive";

static LvcConflictArray combine_workspaces(const std::filesystem::path& lvc, const std::filesystem::path& src_workspace, const std::filesystem::path& dest_workspace, char** error_message) {
    version::prepare_reset(lvc, error_message);
    std::filesystem::path workspace_dir = lvc / NAME_WORKSPACE;
    std::filesystem::path object_dir    = lvc / NAME_OBJECT;

    std::filesystem::path src_workspace_path  = workspace_path(workspace_dir, src_workspace);
    std::filesystem::path dest_workspace_path = workspace_path(workspace_dir, dest_workspace);
    std::vector<std::string> src_version_vector     = io::content_lines(src_workspace_path, 0, error_message);
    std::vector<std::string> dest_version_vector    = io::content_lines(dest_workspace_path, 0, error_message);
    std::string src_id    = src_version_vector[0];
    std::string dest_id    = dest_version_vector[0];
    std::string common_id = latest_common_version(src_version_vector, dest_version_vector);
    std::vector<object::info> src    = version::all_objects(object_dir, src_id, {}, error_message);
    std::vector<object::info> dest   = version::all_objects(object_dir, dest_id, {}, error_message);
    std::vector<object::info> common = version::all_objects(object_dir, common_id, {}, error_message);
    LvcConflictArray out;
    return out;
}

void operation::unite(const std::filesystem::path& lvc, const char* src_workspace, const char* dest_workspace, const char* author, char** error_message) {
    std::filesystem::path workspace_dir = lvc / NAME_WORKSPACE;
    if (workspace::is_inactive(src_workspace)) {
        error_message_creator(src_workspace + inactive_error_message, error_message);
        return;
    }
    if (workspace::is_inactive(dest_workspace)) {
        error_message_creator(dest_workspace + inactive_error_message, error_message);
        return;
    }
    LvcConflictArray conflicts = combine_workspaces(lvc, src_workspace, dest_workspace, error_message);
    std::string message = "unite with workspace: ";
    message += src_workspace;
    version::create(lvc, message.c_str(), author, "", error_message);
}

void operation::insert(const std::filesystem::path& lvc, const char* src_workspace, const char* dest_workspace, const char* author, char** error_message) {
    std::filesystem::path workspace_dir = lvc / NAME_WORKSPACE;
    if (workspace::is_inactive(src_workspace)) {
        error_message_creator(src_workspace + inactive_error_message, error_message);
        return;
    }
    if (workspace::is_inactive(dest_workspace)) {
        error_message_creator(dest_workspace + inactive_error_message, error_message);
        return;
    }
    LvcConflictArray conflicts = combine_workspaces(lvc, src_workspace, dest_workspace, error_message);
    std::string message = "insert workspace ";
    message += src_workspace;
    version::create(lvc, message.c_str(), author, src_workspace, error_message);
}