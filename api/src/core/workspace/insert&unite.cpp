#include <core.hpp>

static LvcConflictArray combine_workspaces(const std::filesystem::path& lvc, const std::filesystem::path& src_workspace, const std::filesystem::path& dest_workspace) {
    version::prepare_reset(lvc);
    std::filesystem::path workspace_dir = lvc / NAME_WORKSPACE;
    std::filesystem::path object_dir    = lvc / NAME_OBJECT;

    std::filesystem::path src_workspace_path  = workspace_path(workspace_dir, src_workspace);
    std::filesystem::path dest_workspace_path = workspace_path(workspace_dir, dest_workspace);
    std::vector<std::string> src_version_vector     = io::content_lines(src_workspace_path, 0);
    std::vector<std::string> dest_version_vector    = io::content_lines(dest_workspace_path, 0);
    std::string src_id    = src_version_vector[0];
    std::string dest_id    = dest_version_vector[0];
    std::string common_id = latest_common_version(src_version_vector, dest_version_vector);
    std::vector<Object> src    = version::all_objects(object_dir, src_id, {});
    std::vector<Object> dest   = version::all_objects(object_dir, dest_id, {});
    std::vector<Object> common = version::all_objects(object_dir, common_id, {});
    
    std::vector<ObjectDiff> diff = object::find_difference(src, dest);
    LvcConflictArray out;
    return out;
}

LvcError workspace::unite(const std::filesystem::path& lvc, const std::string& src_workspace, const std::string& dest_workspace, const std::string& author) {
    std::filesystem::path workspace_dir = lvc / NAME_WORKSPACE;
    if (workspace::is_inactive(src_workspace) || workspace::is_inactive(dest_workspace))
        return UNITE_INACTIVE;
    LvcConflictArray conflicts = combine_workspaces(lvc, src_workspace, dest_workspace);
    std::string message = "unite with workspace: ";
    message += src_workspace;
    version::create(lvc, message.c_str(), author, "");
    return SUCCESS;
}

LvcError workspace::insert(const std::filesystem::path& lvc, const std::string& src_workspace, const std::string& dest_workspace, const std::string& author) {
    std::filesystem::path workspace_dir = lvc / NAME_WORKSPACE;
    if (workspace::is_inactive(src_workspace) || workspace::is_inactive(dest_workspace))
        return INSERT_INACTIVE;
    LvcConflictArray conflicts = combine_workspaces(lvc, src_workspace, dest_workspace);
    std::string message = "insert workspace ";
    message += src_workspace;
    version::create(lvc, message.c_str(), author, src_workspace);
    return SUCCESS;
}