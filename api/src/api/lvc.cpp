#include <core.hpp>
#include <lvc.hpp>

extern "C" LVC_API void lvc_create(LvcCreateInput input, char** error_message) noexcept {
    *error_message            = 0;
    std::filesystem::path lvc = input.location;
    lvc /= ".lvc";
    std::filesystem::path workspace_dir = lvc / NAME_WORKSPACE;
    if(input.clone_repository) repository::clone(input.location, input.clone_repository, input.clone_versioning, error_message);
    if(input.clone_versioning) return;
    repository::create(lvc, error_message);
    if(*error_message) return;
    repository::rename(lvc, input.repository_name, error_message);
    if(*error_message) return;
    category::create(workspace_dir, input.category_name, error_message);
    if(*error_message) return;
    workspace::create(workspace_dir, input.category_name, input.workspace_name, error_message);
    if(*error_message) return;
    workspace::_goto(lvc, input.workspace_name, error_message);
    if(*error_message) return;
    workspace::_default(lvc, input.workspace_name, error_message);
    if(*error_message) return;
}

extern "C" LVC_API void lvc_workspace(const char* lvc, const char* category_name, const char* workspace_name, char** error_message) noexcept {
    *error_message = 0;
    workspace::create(lvc, category_name, workspace_name, error_message);
}

extern "C" LVC_API void lvc_category(const char* lvc, const char* category_name, char** error_message) noexcept {
    *error_message = 0;
    category::create(lvc, category_name, error_message);
}

extern "C" LVC_API void lvc_goto(const char* lvc, const char* workspace_name, char** error_message) noexcept {
    *error_message = 0;
    workspace::_goto(lvc, workspace_name, error_message);
}

extern "C" LVC_API void lvc_default(const char* lvc, const char* workspace_name, char** error_message) noexcept {
    *error_message = 0;
    workspace::_default(lvc, workspace_name, error_message);
}

extern "C" LVC_API char** lvc_checkout(const char* lvc, char** error_message) noexcept {
    *error_message                    = 0;
    std::vector<std::string> checkout = version::checkout(lvc, error_message);
    return strvector_to_charpp(checkout);
}

extern "C" LVC_API char** lvc_status(const char* lvc, char** error_message) noexcept {
    *error_message                       = 0;
    std::filesystem::path lvc_path       = lvc;
    std::string           workspace_name = io::content(lvc_path / NAME_CURRENT, 0, error_message);
    std::filesystem::path workspace      = workspace_path(lvc_path / NAME_WORKSPACE, workspace_name);
    std::string           version_id     = io::content_first_line(workspace, error_message);
    if(*error_message) return 0;
    std::vector<std::string> status = version::status(lvc_path, version_id, error_message);
    return strvector_to_charpp(status);
}

extern "C" LVC_API char** lvc_status_all(const char* lvc, char** error_message) noexcept {
    *error_message                      = 0;
    std::vector<std::string> status_all = version::status_all(lvc, error_message);
    return strvector_to_charpp(status_all);
}

extern "C" LVC_API char** lvc_prepare(const char* lvc, int argc, char* argv[], char** error_message) noexcept {
    *error_message = 0;
    std::vector<std::string> input;
    input.reserve(argc - 2);
    for(size_t i = 2; i < argc; i++) input.push_back(argv[i]);

    char** out;
    version::prepare(lvc, input, &out, error_message);
    return out;
}

extern "C" LVC_API void lvc_prepare_reset(const char* lvc, char** error_message) noexcept {
    *error_message = 0;
    version::prepare_reset(lvc, error_message);
}

extern "C" LVC_API void lvc_version(const char* lvc, const char* message, const char* author, char** error_message) noexcept {
    *error_message = 0;
    version::create(lvc, message, author, "", error_message);
}

extern "C" LVC_API LvcVersion* lvc_history(const char* lvc, const char* workspace, size_t depth, size_t length, bool all, char** error_message) noexcept {
    const std::filesystem::path lvc_dir       = lvc;
    const std::filesystem::path object_dir    = lvc_dir / NAME_OBJECT;
    const std::filesystem::path workspace_dir = lvc_dir / NAME_WORKSPACE;

    if(!workspace::exists(workspace_dir, workspace, error_message)) {
        error_message_creator("Workspace does not exist", error_message);
        return {};
    }

    *error_message = 0;
    if(all)
        return version::history_all(object_dir, workspace_dir, workspace, error_message);
    else
        return version::history(object_dir, workspace_dir, workspace, depth, length, error_message);
}

extern "C" LVC_API void lvc_history_free(LvcVersion* version) noexcept { version::history_free(version); }

extern "C" LVC_API void lvc_conflict_manual(const char* lvc, LvcConflictArray* conflicts, char** error_message) noexcept {
    // TODO
    *error_message = 0;
}

extern "C" LVC_API void lvc_conflict_manual_verify(const char* lvc, LvcConflictArray* conflicts, char** error_message) noexcept {
    // TODO
    *error_message = 0;
}

extern "C" LVC_API void lvc_conflict_manual_sync(const char* lvc, LvcConflictArray* conflicts, char** error_message) noexcept {
    // TODO
    *error_message = 0;
}

extern "C" LVC_API void lvc_sync(const char* lvc, const char* tmp_lvc, LvcConflictArray* conflicts, char** error_message) noexcept {
    // TODO
    *error_message = 0;
}

extern "C" LVC_API void
lvc_unite(const char* lvc, const char* src_workspace_name, const char* dest_workspace_name, LvcConflictArray* conflicts, char** error_message) noexcept {
    // TODO
    *error_message = 0;
}

extern "C" LVC_API void
lvc_insert(const char* lvc, const char* src_workspace_name, const char* dest_workspace_name, LvcConflictArray* conflicts, char** error_message) noexcept {
    // TODO
    *error_message = 0;
}

extern "C" LVC_API void lvc_revert(const char* lvc, char* version_id, size_t input_count, char** input, char** error_message) noexcept {
    *error_message = 0;
    std::vector<std::string> input_vector;
    input_vector.reserve(input_count);
    for(size_t i = 0; i < input_count; i++) input_vector.push_back(input[i]);
    version::revert(lvc, version_id, input_vector, error_message);
}

extern "C" LVC_API void lvc_push_local(const char* lvc, const char* tmp_lvc, char** error_message) noexcept {
    // TODO
    *error_message = 0;
}

extern "C" LVC_API void lvc_push_server(const char* lvc, char** error_message) noexcept {
    // TODO
    *error_message = 0;
}

LVC_API void lvc_rename_category(const char* lvc, const char* category_name, const char* new_name, char** error_message) noexcept {
    *error_message                      = 0;
    std::filesystem::path workspace_dir = lvc;
    workspace_dir /= NAME_WORKSPACE;
    category::rename(workspace_dir, category_name, new_name, error_message);
}

LVC_API void
lvc_move_workspace(const char* lvc, const char* workspace_name, const char* previous_category, const char* category, char** error_message) noexcept {
    *error_message = 0;
    workspace::move_categories(lvc, workspace_name, previous_category, category, error_message);
}

LVC_API void lvc_deactivate(const char* lvc, const char* workspace_name, char** error_message) noexcept {
    *error_message = 0;
    workspace::deactivate(lvc, workspace_name, error_message);
}

extern "C" LVC_API LvcBool lvc_category_exists(const char* lvc, const char* name, char** error_message) noexcept {
    *error_message                 = 0;
    std::filesystem::path lvc_path = lvc;
    return category::exists(lvc_path / NAME_WORKSPACE, name);
}

extern "C" LVC_API LvcBool lvc_workspace_exists(const char* lvc, const char* name, char** error_message) noexcept {
    *error_message                 = 0;
    std::filesystem::path lvc_path = lvc;
    return workspace::exists(lvc_path / NAME_WORKSPACE, name, error_message);
}

extern "C" LVC_API void lvc_free_charpp(char** arr) noexcept { free_charpp(arr); }