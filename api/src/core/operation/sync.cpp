#include "core.hpp"

LvcConflictArray operation::sync(const std::filesystem::path src_repository, const std::filesystem::path dest_repository, char** error_message) {
    const std::filesystem::path src_lvc  = src_repository  / ".lvc";
    const std::filesystem::path dest_lvc = dest_repository / ".lvc";
    const std::filesystem::path src_workspace_directory  = src_lvc  / NAME_WORKSPACE;
    const std::filesystem::path dest_workspace_directory = dest_lvc / NAME_WORKSPACE;
    const std::filesystem::path src_object_directory  = src_lvc  / NAME_OBJECT;
    const std::filesystem::path dest_object_directory = dest_lvc / NAME_OBJECT;

    const std::string workspace = io::content(src_lvc / NAME_CURRENT, 0, error_message);
    const std::filesystem::path operation = src_workspace_directory / NAME_LOCAL / workspace / NAME_OPERATION;
    operation::create_folder(operation, operation::type::sync, error_message);

    std::filesystem::path src_workspace  = workspace_path(src_workspace_directory, workspace); 
    std::filesystem::path dest_workspace = workspace_path(dest_workspace_directory, workspace);
    std::vector<std::string> src_version_vector  = io::content_lines(src_workspace, 0, error_message); 
    std::vector<std::string> dest_version_vector = io::content_lines(dest_workspace, 0, error_message);
    std::string src_id    = src_version_vector[0];
    std::string dest_id    = dest_version_vector[0];
    std::string common_id = latest_common_version(src_version_vector, dest_version_vector);
    std::vector<object::info> src    = version::all_objects(src_object_directory, src_id, {}, error_message);
    std::vector<object::info> dest   = version::all_objects(dest_object_directory, dest_id, {}, error_message);
    std::vector<object::info> common = version::all_objects(src_object_directory, common_id, {}, error_message);

    // on success
    std::filesystem::remove_all(operation);
    return {};
}