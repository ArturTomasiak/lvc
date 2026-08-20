#include "core.hpp"

LvcError workspace::sync(const std::filesystem::path& src_repository, const std::filesystem::path& dest_repository, LvcConflictArray& conflict) {
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
    std::string latest_common_id;
    bool found = 0;

    for (size_t i = 0; i < src_version_vector.size() && !found; i++) {
        for (size_t j = 0; j < dest_version_vector.size() && !found; j++) {
            if (src_version_vector[i] == dest_version_vector[j]) {
                latest_common_id = src_version_vector[i];
                found = 1;
            }
        } 
    }

    std::string latest_src_id  = std::move(src_version_vector[0]); 
    std::string latest_dest_id = std::move(dest_version_vector[0]); 
    if (!found)
        ; // TODO alternative behaviour

   std::vector<Object> latest_common = version::all_objects(src_object_directory, latest_common_id, {});
   std::vector<Object> latest_src    = version::all_objects(src_object_directory, latest_src_id, {});
   std::vector<Object> latest_dest   = version::all_objects(dest_object_directory, latest_dest_id, {});

   return SUCCESS; //TODO
}