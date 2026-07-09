#include <lvc.hpp>
#include <internal.hpp>
#include <unordered_map>

extern "C" LVC_API LvcError lvc_create (LvcCreateInput input) {
    std::filesystem::path lvc = input.location;
    lvc /= ".lvc";
    RETURN_ERR(create::lvc(lvc));
    RETURN_ERR(write::repository_name(lvc, input.repository_name));
    RETURN_ERR(create::category(lvc, input.category_name));
    RETURN_ERR(create::workspace(lvc, input.category_name, input.workspace_name));
    RETURN_ERR(write::workspace_current(lvc, input.workspace_name));
    RETURN_ERR(write::workspace_default(lvc, input.workspace_name));
    return SUCCESS;
}

extern "C" LVC_API LvcError lvc_workspace(const char* lvc, const char* category_name, const char* workspace_name) {
    RETURN_ERR(create::workspace(lvc, category_name, workspace_name));
    return SUCCESS;
}

extern "C" LVC_API LvcError lvc_category(const char* lvc, const char* category_name) {
    RETURN_ERR(create::category(lvc, category_name));
    return SUCCESS;
}

extern "C" LVC_API LvcError lvc_goto(const char* lvc, const char* workspace_name) {
    return SUCCESS;
}

extern "C" LVC_API LvcError lvc_diff(const char* lvc, char*** files) {
    return SUCCESS;
}

extern "C" LVC_API LvcError lvc_status(const char* lvc, char*** files) {
    return SUCCESS;
}

extern "C" LVC_API LvcError lvc_prepare(const char* lvc, int argc, char* argv[]) {
    return SUCCESS;
}

extern "C" LVC_API LvcError lvc_version(const char* lvc, const char* message) {
    return SUCCESS;
}

extern "C" LVC_API LvcError lvc_conflict_manual(const char* lvc, LvcConflictArray* conflicts) {
    return SUCCESS;
}

extern "C" LVC_API LvcError lvc_conflict_manual_verify(const char* lvc, LvcConflictArray* conflicts) {
    return SUCCESS;
}

extern "C" LVC_API LvcError lvc_conflict_manual_sync(const char* lvc, LvcConflictArray* conflicts) {
    return SUCCESS;
}

extern "C" LVC_API LvcError lvc_sync(const char* lvc, const char* tmp_lvc, LvcConflictArray* conflicts){
    return SUCCESS;
}

extern "C" LVC_API LvcError lvc_unite(const char* lvc, const char* workspace_name, LvcConflictArray* conflicts) {
    return SUCCESS;
}

extern "C" LVC_API LvcError lvc_insert(const char* lvc, const char* workspace_name, LvcConflictArray* conflicts) {
    return SUCCESS;
}

extern "C" LVC_API LvcError lvc_revert(const char* lvc, uint32_t version_id, uint32_t file_count, char** files) {
    return SUCCESS;
}

extern "C" LVC_API LvcError lvc_push_local(const char* lvc, const char* tmp_lvc) {
    return SUCCESS;
}

extern "C" LVC_API LvcError lvc_push_server(const char* lvc) {
    return SUCCESS;
}

extern "C" LVC_API LvcBool lvc_category_exists(const char* lvc, const char* name) {
    return exists::category(lvc, name);
}

extern "C" LVC_API LvcBool lvc_workspace_exists(const char* lvc, const char* name) {
    return exists::workspace(lvc, name, 0);
}

static const std::unordered_map<LvcError, const char*> error_strings = {
    { SUCCESS,                     "No error" },
    { LVC_FOLDER_CREATE,           "Could not create .lvc folder" },
    { WORKSPACE_FOLDER_CREATE,     "Could not create .lvc/workspace folder" },
    { OBJECT_FOLDER_CREATE,        "Could not create .lvc/object folder" },
    { CATEGORY_FOLDER_CREATE,      "Could not create .lvc/workspace/<category_name> folder" },
    { WORKSPACE_FILE_CREATE,       "Could not create .lvc/workspace/<category_name>/<workspace_name> file" },
    { CURRENT,                     "Could not modify .lvc/current file" },
    { DEFAULT,                     "Could not modify .lvc/default file" },
    { NAME,                        "Could not modify .lvc/name file" },
    { CATEGORY_EXISTS,             "Category already exists" },
    { CATEGORY_NOT_EXISTS,         "Category doesn't exists" },
    { WORKSPACE_EXISTS,            "Workspace already exists" },
    { WORKSPACE_NOT_EXISTS,        "Workspace doesn't exists" }
};

extern "C" LVC_API const char* lvc_error_string(LvcError error_code) {
    std::unordered_map<LvcError, const char*>::const_iterator it = error_strings.find(error_code);
    if (it != error_strings.end())
        return it->second;
    return "Unknown error";
}