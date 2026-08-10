#pragma once

// REMOVABLE
#define TEST_PRINTS

// MODIFIABLE
#define PREALLOCATE_SMALL 30 // minimize vector allocations
#define PREALLOCATE       30

// DO NOT TOUCH

#include <stdint.h>
#define LvcBool int8_t

#ifdef TEST_PRINTS
#include <iostream>
#endif

#if defined(LVC_STATIC)
    #define LVC_API
#elif defined(_WIN32)
    #if defined(LVC_MAKE_DLL)
        #define LVC_API __declspec(dllexport)lvc
    #else
        #define LVC_API __declspec(dllimport)
    #endif
#elif defined(__GNUC__) || defined(__clang__)
    #define LVC_API __attribute__((visibility("default")))
#else
    #define LVC_API
#endif

#define LVC_RETURN_IF_ERROR(function)    \
do {                                     \
    LvcError err = (function);           \
    if (err)                             \
        return err;                      \
} while (0)

#ifdef __cplusplus
extern "C" {
#endif

    typedef enum LvcError {
        SUCCESS,

        FILE_CREATION_FAILURE,
        FILE_WRITING_FAILURE,
        FILE_READING_FAILURE,
        FILESYSTEM_COPY_ERROR,
        CLONE_NO_LVC,

        DEFLATION_FAILURE,
        INFLATION_FAILURE,
        
        LVC_FOLDER_CREATE, 
        WORKSPACE_FOLDER_CREATE,
        OBJECT_FOLDER_CREATE,
        CATEGORY_FOLDER_CREATE,
        CATEGORY_FOLDER_RENAME,
        WORKSPACE_FILE_CREATE,
        WORKSPACE_MOVE_CATEGORIES,
        WORKSPACE_DEACTIVATE,
        OBJECT_FILE_CREATE,
        CURRENT,
        DEFAULT,
        NAME,
        CATEGORY_EXISTS,
        CATEGORY_NOT_EXISTS,
        WORKSPACE_EXISTS,
        WORKSPACE_NOT_EXISTS,
        PREPARE_NO_INPUT,
        MEMORY_ALLOCATION_FAILED,
        VERSION_NO_MESSAGE,
        PREPARE_RESET_ERROR,
        WORKING_DIR_ITERATION_FAILED,

        CREATE_WORKSPACE_INACTIVE,
        GOTO_INACTIVE,
        DEFAULT_INACTIVE,
        MOVE_INACTIVE,
        UNITE_INACTIVE,
        INSERT_INACTIVE
    } LvcError;

    typedef enum StorageBehaviour {
        DISTRIBUTED = 1,
        HYBRID = 2,
        CENTRALIZED = 3
    } StorageBehaviour;
    
    typedef struct LvcCreateInput {
        LvcBool local;
        StorageBehaviour storage_behaviour;
        const char* location;
        const char* server_link;
        const char* repository_name;
        const char* category_name;
        const char* workspace_name;
        const char* clone_repository;
        bool clone_versioning;
    } LvcCreateInput;

    typedef struct LvcDiff {
        uint32_t line_start;
        uint32_t line_end;
        const char* content;
    } LvcDiff;

    typedef struct LvcConflict {
        LvcDiff a;
        LvcDiff b;
    } LvcConflict;

    typedef struct LvcConflictArray {
        uint32_t length;
        LvcConflict* conflict;
        const char* absolute_path;
    } LvcConflictArray;

    LVC_API LvcError lvc_create(LvcCreateInput input) noexcept;
    LVC_API LvcError lvc_workspace(const char* lvc, const char* category_name, const char* workspace_name) noexcept;
    LVC_API LvcError lvc_category(const char* lvc, const char* category_name) noexcept;
    LVC_API LvcError lvc_goto(const char* lvc, const char* workspace_name) noexcept;
    LVC_API LvcError lvc_default(const char* lvc, const char* workspace_name) noexcept;
    LVC_API char**   lvc_diff(const char* lvc, LvcError* err) noexcept;
    LVC_API char**   lvc_status(const char* lvc) noexcept;
    LVC_API char**   lvc_status_all(const char* lvc) noexcept;
    LVC_API char**   lvc_prepare(const char* lvc, int argc, char* argv[], LvcError* err) noexcept;
    LVC_API LvcError lvc_prepare_reset(const char* lvc) noexcept;
    LVC_API LvcError lvc_version(const char* lvc, const char* message, const char* author) noexcept;
    LVC_API LvcError lvc_conflict_manual(const char* lvc, LvcConflictArray* conflicts) noexcept;
    LVC_API LvcError lvc_conflict_manual_verify(const char* lvc, LvcConflictArray* conflicts) noexcept;
    LVC_API LvcError lvc_conflict_manual_sync(const char* lvc, LvcConflictArray* conflicts) noexcept;
    LVC_API LvcError lvc_sync(const char* lvc, const char* tmp_lvc, LvcConflictArray* conflicts) noexcept;
    LVC_API LvcError lvc_unite(const char* lvc, const char* src_workspace_name, const char* dest_workspace_name, LvcConflictArray* conflicts) noexcept;
    LVC_API LvcError lvc_insert(const char* lvc, const char* src_workspace_name, const char* dest_workspace_name, LvcConflictArray* conflicts) noexcept;
    LVC_API LvcError lvc_revert(const char* lvc, uint32_t version_id, uint32_t file_count, char** files) noexcept;
    LVC_API LvcError lvc_push_local(const char* lvc, const char* tmp_lvc) noexcept;
    LVC_API LvcError lvc_push_server(const char* lvc) noexcept;
    LVC_API LvcError lvc_rename_category(const char* lvc, const char* category_name, const char* new_name) noexcept;
    LVC_API LvcError lvc_move_workspace(const char* lvc, const char* workspace_name, const char* previous_category, const char* category) noexcept;
    LVC_API LvcError lvc_deactivate(const char* lvc, const char* workspace_name) noexcept;
    LVC_API LvcBool lvc_category_exists(const char* lvc, const char* name) noexcept;
    LVC_API LvcBool lvc_workspace_exists(const char* lvc, const char* name, LvcError* err) noexcept;
    LVC_API const char* lvc_error_string(LvcError error_code) noexcept;
    LVC_API void lvc_free_charpp(char** arr) noexcept;
#ifdef __cplusplus
}
#endif

// defines which line of deflated version has what content
#define VERSION_TYPE      0
#define VERSION_ROOT_TREE 1
#define VERSION_MESSAGE   2
#define VERSION_AUTHOR    3
#define VERSION_WORKSPACE 4 

// file/folder name defines 
#define NAME_WORKSPACE "workspace"
#define NAME_INACTIVE "inactive"
#define NAME_OBJECT "object"
#define NAME_STATUS "status"
#define NAME_DEFAULT "default"
#define NAME_CURRENT "current"
#define NAME_PREPARE "prepare"
#define NAME_NAME "name"
#define NAME_IGNORE "lvc.ignore"
#define NAME_STORAGE "lvc.storage"

// type string defines
#define TYPE_VERSION "version"
#define TYPE_TREE "tree"
#define TYPE_BLOB "blob"

// prefixes

#define PREFIX_DELETED "%DELETED% "