#include <core.hpp>

// typedef struct LvcDiff {
//     uint32_t line_start;
//     uint32_t line_end;
//     const char* content;
// } LvcDiff;

// typedef struct LvcConflict {
//     LvcDiff a;
//     LvcDiff b;
// } LvcConflict;

// typedef struct LvcConflictArray {
//     uint32_t length;
//     LvcConflict* conflict;
//     const char* absolute_path;
// } LvcConflictArray;

// TODO

static LvcConflictArray combine_workspaces(const std::filesystem::path& lvc, const std::filesystem::path& src_workspace, const std::filesystem::path& dest_workspace) {
    LvcConflictArray out;
    return out;
}

LvcError workspace::unite(const std::filesystem::path& lvc, const std::filesystem::path& src_workspace, const std::filesystem::path& dest_workspace, const std::string& author) {
    std::filesystem::path workspace_dir = lvc / NAME_WORKSPACE;
    if (workspace::is_inactive(src_workspace) || workspace::is_inactive(dest_workspace))
        return UNITE_INACTIVE;
    LvcConflictArray conflicts = combine_workspaces(lvc, src_workspace, dest_workspace);
    std::string message = "unite with workspace: ";
    message += src_workspace;
    version::create(lvc, message.c_str(), author, src_workspace);
    return SUCCESS;
}

LvcError workspace::insert(const std::filesystem::path& lvc, const std::filesystem::path& src_workspace, const std::filesystem::path& dest_workspace, const std::string& author) {
    std::filesystem::path workspace_dir = lvc / NAME_WORKSPACE;
    if (workspace::is_inactive(src_workspace) || workspace::is_inactive(dest_workspace))
        return INSERT_INACTIVE;
    LvcConflictArray conflicts = combine_workspaces(lvc, src_workspace, dest_workspace);
    std::string message = "insert workspace ";
    message += src_workspace;
    version::create(lvc, message.c_str(), author, src_workspace);
    return SUCCESS;
}