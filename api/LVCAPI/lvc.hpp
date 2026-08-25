#pragma once

// REMOVABLE
#define TEST_PRINTS

// MODIFIABLE
#define PREALLOCATE_SMALL 10 // minimize vector allocations
#define PREALLOCATE       30

// DO NOT TOUCH

#include <stdint.h>
#include <stddef.h>
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

#ifdef __cplusplus
extern "C" {
#endif
    typedef enum {
        DISTRIBUTED = 1,
        HYBRID = 2,
        CENTRALIZED = 3
    } StorageBehaviour;
    
    typedef struct {
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

    typedef struct {
        size_t line_start;
        size_t line_end;
        const char* content;
    } LvcDiff;

    typedef struct {
        LvcDiff a;
        LvcDiff b;
        const char* relative_path;
    } LvcConflict;

    typedef struct {
        size_t length;
        LvcConflict* conflict;
    } LvcConflictArray;

    typedef struct LvcVersion {
        LvcVersion* previous;
        LvcVersion* nested_versions;
        char* description;
        char  id[65];
    } LvcVersion;

    LVC_API void lvc_create(LvcCreateInput input, char** error_message) noexcept;
    LVC_API void lvc_workspace(const char* lvc, const char* category_name, const char* workspace_name, char** error_message) noexcept;
    LVC_API void lvc_category(const char* lvc, const char* category_name, char** error_message) noexcept;
    LVC_API void lvc_goto(const char* lvc, const char* workspace_name, char** error_message) noexcept;
    LVC_API void lvc_default(const char* lvc, const char* workspace_name, char** error_message) noexcept;
    LVC_API char**   lvc_checkout(const char* lvc, char** error_message) noexcept;
    LVC_API char**   lvc_status(const char* lvc, char** error_message) noexcept;
    LVC_API char**   lvc_status_all(const char* lvc, char** error_message) noexcept;
    LVC_API char**   lvc_prepare(const char* lvc, int argc, char* argv[], char** error_message) noexcept;
    LVC_API void lvc_prepare_reset(const char* lvc, char** error_message) noexcept;
    LVC_API void lvc_version(const char* lvc, const char* message, const char* author, char** error_message) noexcept;
    LVC_API LvcVersion* lvc_history(const char* lvc, const char* workspace, size_t depth, size_t length, bool all, char** error_message) noexcept;
    LVC_API void        lvc_history_free(LvcVersion* version) noexcept;
    LVC_API void lvc_conflict_manual(const char* lvc, LvcConflictArray* conflicts, char** error_message) noexcept;
    LVC_API void lvc_conflict_manual_verify(const char* lvc, LvcConflictArray* conflicts, char** error_message) noexcept;
    LVC_API void lvc_conflict_manual_sync(const char* lvc, LvcConflictArray* conflicts, char** error_message) noexcept;
    LVC_API void lvc_sync(const char* lvc, const char* tmp_lvc, LvcConflictArray* conflicts, char** error_message) noexcept;
    LVC_API void lvc_unite(const char* lvc, const char* src_workspace_name, const char* dest_workspace_name, LvcConflictArray* conflicts, char** error_message) noexcept;
    LVC_API void lvc_insert(const char* lvc, const char* src_workspace_name, const char* dest_workspace_name, LvcConflictArray* conflicts, char** error_message) noexcept;
    LVC_API void lvc_revert(const char* lvc, char* version_id, size_t input_count, char** input, char** error_message) noexcept;
    LVC_API void lvc_push_local(const char* lvc, const char* tmp_lvc, char** error_message) noexcept;
    LVC_API void lvc_push_server(const char* lvc, char** error_message) noexcept;
    LVC_API void lvc_rename_category(const char* lvc, const char* category_name, const char* new_name, char** error_message) noexcept;
    LVC_API void lvc_move_workspace(const char* lvc, const char* workspace_name, const char* previous_category, const char* category, char** error_message) noexcept;
    LVC_API void lvc_deactivate(const char* lvc, const char* workspace_name, char** error_message) noexcept;
    LVC_API LvcBool lvc_category_exists(const char* lvc, const char* name, char** error_message) noexcept;
    LVC_API LvcBool lvc_workspace_exists(const char* lvc, const char* name, char** error_message) noexcept;
    LVC_API void lvc_free_charpp(char** arr) noexcept;
#ifdef __cplusplus
}
#endif

// defines which line of deflated version has what content
#define VERSION_PATTERN   0
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