#include "core.hpp"

LvcConflictArray repository::sync(const std::filesystem::path src_repository, const std::filesystem::path dest_repository, char** error_message) {
    std::filesystem::path src_lvc  = src_repository  / ".lvc";
    std::filesystem::path dest_lvc = dest_repository / ".lvc";
    std::filesystem::path src_workspace_directory  = src_lvc  / NAME_WORKSPACE;
    std::filesystem::path dest_workspace_directory = dest_lvc / NAME_WORKSPACE;
    std::filesystem::path src_object_directory  = src_lvc  / NAME_OBJECT;
    std::filesystem::path dest_object_directory = dest_lvc / NAME_OBJECT;

    std::string workspace = io::content(src_lvc / NAME_CURRENT, 0, error_message);

    std::filesystem::path src_workspace  = workspace_path(src_workspace_directory, workspace); 
    std::filesystem::path dest_workspace = workspace_path(dest_workspace_directory, workspace);
    std::vector<std::string> src_version_vector  = io::content_lines(src_workspace, 0, error_message); 
    std::vector<std::string> dest_version_vector = io::content_lines(dest_workspace, 0, error_message);
    std::string src_id    = src_version_vector[0];
    std::string dest_id    = dest_version_vector[0];
    std::string common_id = latest_common_version(src_version_vector, dest_version_vector);
    std::vector<Object> src    = version::all_objects(src_object_directory, src_id, {}, error_message);
    std::vector<Object> dest   = version::all_objects(dest_object_directory, dest_id, {}, error_message);
    std::vector<Object> common = version::all_objects(src_object_directory, common_id, {}, error_message);

    std::vector<ObjectDiff> diff = object::find_difference(src, dest);
    return {};
}