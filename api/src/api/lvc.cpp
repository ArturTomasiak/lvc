#include <chrono>
#include <core.hpp>
#include <lvc.hpp>
#include <test.hpp>

extern "C" LVC_API void lvc_create(LvcCreateInput input, char** error_message) noexcept {
    std::filesystem::path lvc = input.location;
    lvc /= ".lvc";
    Paths paths(lvc.c_str());
    if (std::filesystem::is_directory(paths.lvc)) {
        error_message_creator("Repository already exists", error_message);
        return;
    }
    if (input.clone_repository)
        repository::clone(paths, input.clone_repository, input.clone_versioning, error_message);
    if (input.clone_versioning)
        return;
    repository::create(paths, error_message);
    if (*error_message)
        return;
    repository::rename(paths, input.repository_name, error_message);
    if (*error_message)
        return;
    workspace::create(paths, input.workspace_name, std::string{}, error_message);
    if (*error_message)
        return;
    io::file(
        paths.current, std::ios::binary | std::ios::trunc, input.workspace_name, strlen(input.workspace_name), 0,
        error_message);
    if (*error_message)
        return;
    workspace::_default(paths, input.workspace_name, error_message);
    if (*error_message)
        return;
}

extern "C" LVC_API char* lvc_current(const char* lvc, char** error_message) noexcept {
    Paths       paths(lvc);
    std::string current = io::content(paths.current, 0, 0, error_message);
    char*       out     = (char*)malloc(current.size() + 1);
    memcpy(out, current.data(), current.size());
    out[current.size()] = '\0';
    return out;
}

extern "C" LVC_API void
lvc_workspace(const char* lvc, const char* workspace_name, LVC_BOOL clean, char** error_message) noexcept {
    Paths       paths(lvc);
    std::string cpy;
    if (!clean)
        cpy = io::content(paths.current, 0, 0, error_message);
    workspace::create(paths, workspace_name, cpy, error_message);
}

extern "C" LVC_API void lvc_change_category(
    const char* lvc, const char* workspace_name, const char* category_name, char** error_message) noexcept {
    Paths paths(lvc);
    category::change(paths.workspace / workspace_name, category_name, error_message);
}

extern "C" LVC_API void lvc_goto(const char* lvc, const char* workspace_name, char** error_message) noexcept {
    Paths paths(lvc);
    workspace::_goto(paths, workspace_name, error_message);
}

extern "C" LVC_API void lvc_default(const char* lvc, const char* workspace_name, char** error_message) noexcept {
    Paths paths(lvc);
    workspace::_default(paths, workspace_name, error_message);
}

extern "C" LVC_API char** lvc_checkout(const char* lvc, char** error_message) noexcept {
    Paths                    paths(lvc);
    std::vector<std::string> checkout = version::checkout(paths, error_message);
    return strvector_to_charpp(checkout);
}

extern "C" LVC_API char** lvc_status(const char* lvc, char** error_message) noexcept {
    Paths                          paths(lvc);
    const std::string              workspace_name    = io::content(paths.current, 0, 0, error_message);
    const std::filesystem::path    workspace         = paths.workspace / workspace_name;
    const std::filesystem::path    prepare           = prepare_path(paths, workspace);
    const std::vector<std::string> workspace_content = io::content_lines(workspace, 1, 1, error_message);
    const std::string              version_id        = workspace_content.empty() ? "" : workspace_content[0];
    if (*error_message)
        return 0;
    std::vector<std::string> status = version::status(paths, prepare, version_id, error_message);
    return strvector_to_charpp(status);
}

extern "C" LVC_API char** lvc_status_all(const char* lvc, char** error_message) noexcept {
    Paths                    paths(lvc);
    std::vector<std::string> status_all = version::status_all(paths, error_message);
    return strvector_to_charpp(status_all);
}

extern "C" LVC_API char** lvc_prepare(const char* lvc, int argc, char* argv[], char** error_message) noexcept {
    Paths                    paths(lvc);
    std::vector<std::string> input;
    input.reserve(argc - 2);
    for (size_t i = 2; i < argc; i++)
        input.push_back(argv[i]);

    char** out;
    version::prepare(paths, input, &out, error_message);
    return out;
}

extern "C" LVC_API void lvc_prepare_reset(const char* lvc, char** error_message) noexcept {
    Paths paths(lvc);
    version::prepare_reset(paths, error_message);
}

extern "C" LVC_API void
lvc_version(const char* lvc, const char* message, const char* author, char** error_message) noexcept {
    Paths paths(lvc);
    version::create(paths, message, author, error_message);
}

extern "C" LVC_API LvcCategoryList* lvc_category_list(const char* lvc, char** error_message) noexcept {
    Paths paths(lvc);
    return category::list(paths, error_message);
}

extern "C" LVC_API void lvc_category_list_free(LvcCategoryList* list) noexcept { category::free_category_list(list); }

extern "C" LVC_API LvcVersion* lvc_history(const char* lvc, const char* workspace, char** error_message) noexcept {
    Paths paths(lvc);
    if (!workspace::exists(paths, workspace, error_message)) {
        error_message_creator("Workspace does not exist", error_message);
        return {};
    }

    return version::history(paths, workspace, error_message);
}

extern "C" LVC_API void lvc_history_free(LvcVersion* version) noexcept { version::history_free(version); }

extern "C" LVC_API void lvc_terminal_conflict_solver(const char* lvc, char** error_message) noexcept {
    // TODO
    Paths paths(lvc);
}

extern "C" LVC_API void lvc_diff(const char* lvc, int argc, char* argv[], char** error_message) noexcept {
    Paths paths(lvc);

    const std::string              workspace_name    = io::content(paths.current, 0, 0, error_message);
    const std::filesystem::path    workspace         = paths.workspace / workspace_name;
    const std::vector<std::string> workspace_content = io::content_lines(workspace, 1, 1, error_message);
    const std::string              version_id        = workspace_content.empty() ? "" : workspace_content[0];

    std::vector<std::string> raw_input;
    raw_input.reserve(argc - 2);
    for (size_t i = 2; i < argc; i++)
        raw_input.push_back(argv[i]);

    std::vector<std::string> input = paths.resolve(raw_input, version_id, 1, 0, error_message);

    std::unordered_map<std::filesystem::path, object::info> version =
        version::map_all_objects(paths, version_id, {}, error_message);

    for (std::string& path : input) {
        bool                                                              deleted = path.starts_with(PREFIX_DELETED);
        std::unordered_map<std::filesystem::path, object::info>::iterator it      = version.find(path);
        if (deleted) {
            path.erase(0, strlen(PREFIX_DELETED));
            std::cout << "Deleted " + path + "\n";
        } else if (it == version.end()) {
            std::cout << "Created " + path + "\n";
        } else {
            object::info&                  info = it->second;
            const std::vector<std::string> base = io::content_lines(paths.object / info.id, 1, 1, error_message);
            const std::vector<std::string> side = io::content_lines(path, 0, 0, error_message);
            print_diff(base, side, path);
        }
    }
}

extern "C" LVC_API LvcConflictArray* lvc_sync(const char* lvc, const char* tmp_lvc, char** error_message) noexcept {
    // TODO
    Paths paths(lvc);
    return NULL;
}

extern "C" LVC_API LvcConflictArray*
lvc_merge(const char* lvc, const char* src_workspace_name, const char* author, char** error_message) noexcept {
    Paths             paths(lvc);
    const std::string dest_workspace_name       = io::content(paths.current, 0, 0, error_message);
    const std::string src_workspace_name_string = src_workspace_name;
    return operation::merge(paths, src_workspace_name_string, dest_workspace_name, author, error_message);
}

extern "C" LVC_API void
lvc_revert(const char* lvc, char* version_id, size_t input_count, char** input, char** error_message) noexcept {
    Paths                    paths(lvc);
    std::vector<std::string> input_vector;
    input_vector.reserve(input_count);
    for (size_t i = 0; i < input_count; i++)
        input_vector.push_back(input[i]);
    operation::revert(paths, version_id, input_vector, error_message);
}

extern "C" LVC_API void lvc_push_local(const char* lvc, const char* tmp_lvc, char** error_message) noexcept {
    // TODO
    Paths paths(lvc);
}

extern "C" LVC_API void lvc_push_server(const char* lvc, char** error_message) noexcept {
    // TODO
    Paths paths(lvc);
}

extern "C" LVC_API LVC_BOOL lvc_version_exists(const char* lvc, const char* id) noexcept {
    std::filesystem::path lvc_path = lvc;
    return std::filesystem::is_regular_file(lvc_path / foldername_object / id);
}

extern "C" LVC_API LVC_BOOL lvc_workspace_exists(const char* lvc, const char* name, char** error_message) noexcept {
    Paths paths(lvc);
    return workspace::exists(paths, name, error_message);
}

extern "C" LVC_API void lvc_free_charpp(char** arr) noexcept { free_charpp(arr); }

extern "C" LVC_API void lvc_run_tests() noexcept {
    if (test::diff())
        std::cout << "Diff tests passed\n";
    if (test::merge())
        std::cout << "Merge tests passed\n";
    if (test::sha256())
        std::cout << "Sha256 tests passed\n";
}