#include <lvc.hpp>
#include <core.hpp>
#include <unordered_map>

extern "C" LVC_API LvcError lvc_create (LvcCreateInput input) noexcept {
    std::filesystem::path lvc = input.location;
    lvc /= ".lvc";
    std::filesystem::path workspace_dir = lvc / NAME_WORKSPACE;
    if (input.clone_repository)
        RETURN_ERR(repository::clone(input.location, input.clone_repository, input.clone_versioning));
    if (input.clone_versioning)
        return SUCCESS;
    RETURN_ERR(repository::create(lvc));
    RETURN_ERR(repository::rename(lvc, input.repository_name));
    RETURN_ERR(category::create(workspace_dir, input.category_name));
    RETURN_ERR(workspace::create(workspace_dir, input.category_name, input.workspace_name));
    RETURN_ERR(workspace::_goto(lvc, input.workspace_name));
    RETURN_ERR(workspace::_default(lvc, input.workspace_name));
    return SUCCESS;
}

extern "C" LVC_API LvcError lvc_workspace(const char* lvc, const char* category_name, const char* workspace_name) noexcept {
    RETURN_ERR(workspace::create(lvc, category_name, workspace_name));
    return SUCCESS;
}

extern "C" LVC_API LvcError lvc_category(const char* lvc, const char* category_name) noexcept {
    RETURN_ERR(category::create(lvc, category_name));
    return SUCCESS;
}

extern "C" LVC_API LvcError lvc_goto(const char* lvc, const char* workspace_name) noexcept {
    return workspace::_goto(lvc, workspace_name);
}

extern "C" LVC_API LvcError lvc_default(const char* lvc, const char* workspace_name) noexcept {
    return workspace::_default(lvc, workspace_name);
}

extern "C" LVC_API char** lvc_diff(const char* lvc, LvcError* err) noexcept {
    std::vector<std::string> diff = version::diff(lvc, *err);
    return strvector_to_charpp(diff);
}

extern "C" LVC_API char** lvc_status(const char* lvc) noexcept {
    std::vector<std::string> status = version::status(lvc);
    return strvector_to_charpp(status);
}

extern "C" LVC_API char** lvc_status_all(const char* lvc) noexcept {
    std::vector<std::string> status_all = version::status_all(lvc);
    return strvector_to_charpp(status_all);
}

extern "C" LVC_API char** lvc_prepare(const char* lvc, int argc, char* argv[], LvcError* err) noexcept {
    std::vector<std::string> input;
    input.reserve(argc - 2);
    for (size_t i = 2; i < argc; i++)
        input.push_back(argv[i]);

    char** out;
    *err = version::prepare(lvc, input, &out);
    return out;
}

extern "C" LVC_API LvcError lvc_prepare_reset(const char* lvc) noexcept {
    return version::prepare_reset(lvc);
}

extern "C" LVC_API LvcError lvc_version(const char* lvc, const char* message, const char* author) noexcept {
    return version::create(lvc, message, author, "");
}

extern "C" LVC_API LvcVersion* lvc_history(const char* lvc, const char* workspace) noexcept {
    const std::filesystem::path lvc_dir       = lvc;
    const std::filesystem::path object_dir    = lvc_dir / NAME_OBJECT;
    const std::filesystem::path workspace_dir = lvc_dir / NAME_WORKSPACE;
    return version::history(lvc_dir, object_dir, workspace_dir, workspace);
}

extern "C" LVC_API void lvc_history_free(LvcVersion* version) noexcept {
    version::history_free(version);
}

extern "C" LVC_API LvcError lvc_conflict_manual(const char* lvc, LvcConflictArray* conflicts) noexcept {
    // TODO
    return SUCCESS;
}

extern "C" LVC_API LvcError lvc_conflict_manual_verify(const char* lvc, LvcConflictArray* conflicts) noexcept {
    // TODO
    return SUCCESS;
}

extern "C" LVC_API LvcError lvc_conflict_manual_sync(const char* lvc, LvcConflictArray* conflicts) noexcept {
    // TODO
    return SUCCESS;
}

extern "C" LVC_API LvcError lvc_sync(const char* lvc, const char* tmp_lvc, LvcConflictArray* conflicts) noexcept {
    // TODO
    return SUCCESS;
}

extern "C" LVC_API LvcError lvc_unite(const char* lvc, const char* src_workspace_name, const char* dest_workspace_name, LvcConflictArray* conflicts) noexcept {
    // TODO
    return SUCCESS;
}

extern "C" LVC_API LvcError lvc_insert(const char* lvc, const char* src_workspace_name, const char* dest_workspace_name, LvcConflictArray* conflicts) noexcept {
    // TODO
    return SUCCESS;
}

extern "C" LVC_API LvcError lvc_revert(const char* lvc, uint32_t version_id, uint32_t file_count, char** files) noexcept {
    // TODO
    return SUCCESS;
}

extern "C" LVC_API LvcError lvc_push_local(const char* lvc, const char* tmp_lvc) noexcept {
    // TODO
    return SUCCESS;
}

extern "C" LVC_API LvcError lvc_push_server(const char* lvc) noexcept {
    // TODO
    return SUCCESS;
}

LVC_API LvcError lvc_rename_category(const char* lvc, const char* category_name, const char* new_name) noexcept {
    std::filesystem::path workspace_dir = lvc;
    workspace_dir /= NAME_WORKSPACE;
    return category::rename(workspace_dir, category_name, new_name);
}

LVC_API LvcError lvc_move_workspace(const char* lvc, const char* workspace_name, const char* previous_category, const char* category) noexcept {
    return workspace::move_categories(lvc, workspace_name, previous_category, category);
}

LVC_API LvcError lvc_deactivate(const char* lvc, const char* workspace_name) noexcept {
    return workspace::deactivate(lvc, workspace_name);
}

extern "C" LVC_API LvcBool lvc_category_exists(const char* lvc, const char* name) noexcept {
    std::filesystem::path lvc_path = lvc;
    return category::exists(lvc_path / NAME_WORKSPACE, name);
}

extern "C" LVC_API LvcBool lvc_workspace_exists(const char* lvc, const char* name, LvcError* err) noexcept {
    std::filesystem::path lvc_path = lvc;
    return workspace::exists(lvc_path / NAME_WORKSPACE, name, *err);
}

static const std::unordered_map<LvcError, const char*> error_strings = {
    { SUCCESS,                      "No error" },

    { FILE_CREATION_FAILURE,        "Failed to create a file" },
    { FILE_WRITING_FAILURE,         "Failed to write a file" },
    { FILE_READING_FAILURE,         "Failed to read a file" },
    { FILESYSTEM_COPY_ERROR,        "Failed to copy a folder or file" },
    { CLONE_NO_LVC,                 "Cloned local repository must be a path to a folder with a valid .lvc folder" },

    { DEFLATION_FAILURE,            "Failed to deflate file" },
    { INFLATION_FAILURE,            "Failed to inflate file" },

    { LVC_FOLDER_CREATE,            "Could not create .lvc folder" },
    { WORKSPACE_FOLDER_CREATE,      "Could not create .lvc/workspace folder" },
    { OBJECT_FOLDER_CREATE,         "Could not create .lvc/object folder" },
    { CATEGORY_FOLDER_CREATE,       "Could not create .lvc/workspace/<category_name> folder" },
    { CATEGORY_FOLDER_RENAME,       "Could not rename .lvc/workspace/<category_name> folder" },
    { WORKSPACE_FILE_CREATE,        "Could not create .lvc/workspace/<category_name>/<workspace_name> file" },
    { WORKSPACE_MOVE_CATEGORIES,    "Could not move file to .lvc/workspace/<category_name>" },
    { WORKSPACE_DEACTIVATE,         "Could not move file to .lvc/workspace/inactive" },
    { OBJECT_FILE_CREATE,           "Could not create a file in the .lvc/object directory" },
    { CURRENT,                      "Could not modify .lvc/current file" },
    { DEFAULT,                      "Could not modify .lvc/default file" },
    { NAME,                         "Could not modify .lvc/name file" },
    { CATEGORY_EXISTS,              "Category already exists" },
    { CATEGORY_NOT_EXISTS,          "Category doesn't exists" },
    { WORKSPACE_EXISTS,             "Workspace already exists" },
    { WORKSPACE_NOT_EXISTS,         "Workspace doesn't exists" },
    { PREPARE_NO_INPUT,             "Input didn't match to a single file or directory" },
    { MEMORY_ALLOCATION_FAILED,     "Memory allocation failed" },
    { VERSION_NO_MESSAGE,           "A message is required for creating a new version" },
    { PREPARE_RESET_ERROR,          "Could not delete .lvc/prepare" },
    { WORKING_DIR_ITERATION_FAILED, "Could not iterate through working directory. Check file permissions." },

    { CREATE_WORKSPACE_INACTIVE, "Inactive category is reserved and cannot be used for workspace creation" },
    { GOTO_INACTIVE,             "Cannot goto inactive workspace" },
    { DEFAULT_INACTIVE,          "Cannot make an inactive workspace default" },
    { MOVE_INACTIVE,             "Cannot move to or from inactive category. To deactivate/activate a workspace, use lvc deactivate or lvc activate" },
    { UNITE_INACTIVE,            "Cannot unite inactive workspaces" },
    { INSERT_INACTIVE,           "Cannot insert an inactive workspace" }
};

extern "C" LVC_API const char* lvc_error_string(LvcError error_code) noexcept {
    std::unordered_map<LvcError, const char*>::const_iterator it = error_strings.find(error_code);
    if (it != error_strings.end())
        return it->second;
    return "Unknown error";
}

extern "C" LVC_API void lvc_free_charpp(char** arr) noexcept {
    free_charpp(arr);
}