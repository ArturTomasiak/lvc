#include "core.hpp"

static std::string latest_common_version(const std::vector<std::string>& a, const std::vector<std::string>& b) {
    for (size_t i = 0; i < a.size(); i++)
        for (size_t j = 0; j < b.size(); j++)
            if (a[i] == b[j])
                return a[i];
    return "";
}

LvcConflictArray workspace::conflict(const std::filesystem::path lvc, const std::string src_workspace, const std::filesystem::path dest_workspace) {
    std::filesystem::path workspace_dir = lvc / NAME_WORKSPACE;
    std::filesystem::path object_dir    = lvc / NAME_OBJECT;

    std::filesystem::path src_workspace_path  = workspace_path(workspace_dir, src_workspace);
    std::filesystem::path dest_workspace_path = workspace_path(workspace_dir, dest_workspace);
    std::vector<std::string> src_versions     = io::content_lines(src_workspace_path, 0);
    std::vector<std::string> dest_versions    = io::content_lines(dest_workspace_path, 0);
    std::string latest_common = latest_common_version(src_versions, dest_versions);
    std::string latest_src  = std::move(src_versions[0]); 
    std::string latest_dest = std::move(dest_versions[0]); 
    //TODO
    return {};
}

LvcConflictArray workspace::conflict_push(const std::filesystem::path src_repository, const std::filesystem::path dest_repository) {
    std::filesystem::path src_lvc  = src_repository  / ".lvc";
    std::filesystem::path dest_lvc = dest_repository / ".lvc";
    std::filesystem::path src_workspace_directory  = src_lvc  / NAME_WORKSPACE;
    std::filesystem::path dest_workspace_directory = dest_lvc / NAME_WORKSPACE;
    std::filesystem::path src_object_directory  = src_lvc  / NAME_OBJECT;
    std::filesystem::path dest_object_directory = dest_lvc / NAME_OBJECT;
    
    std::string workspace = io::content(src_lvc / NAME_CURRENT, 0);
    
    std::filesystem::path src_workspace  = workspace_path(src_workspace_directory, workspace); 
    std::filesystem::path dest_workspace = workspace_path(dest_workspace_directory, workspace);
    std::vector<std::string> src_version_vector  = io::content_lines(src_workspace, 0); 
    std::vector<std::string> dest_version_vector = io::content_lines(dest_workspace, 0); 
    std::string latest_common = latest_common_version(src_version_vector, dest_version_vector);
    std::string latest_src  = std::move(src_version_vector[0]); 
    std::string latest_dest = std::move(dest_version_vector[0]); 
   //TODO
   return {};
}