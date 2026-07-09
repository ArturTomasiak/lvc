#pragma once

#include <stdint.h>
#define LvcBool int8_t

#if defined(LVC_STATIC)
    #define LVC_API
#elif defined(_WIN32)
    #if defined(LVC_MAKE_DLL)
        #define LVC_API __declspec(dllexport)
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
        LVC_FOLDER_CREATE, 
        WORKSPACE_FOLDER_CREATE,
        OBJECT_FOLDER_CREATE,
        CATEGORY_FOLDER_CREATE,
        WORKSPACE_FILE_CREATE,
        CURRENT,
        DEFAULT,
        NAME,
        CATEGORY_EXISTS,
        CATEGORY_NOT_EXISTS,
        WORKSPACE_EXISTS,
        WORKSPACE_NOT_EXISTS
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
    } LvcConflictArray;

    LVC_API LvcError lvc_create(LvcCreateInput input);
    LVC_API LvcError lvc_workspace(const char* lvc, const char* category_name, const char* workspace_name);
    LVC_API LvcError lvc_category(const char* lvc, const char* category_name);
    LVC_API LvcError lvc_goto(const char* lvc, const char* workspace_name);
    LVC_API LvcError lvc_diff(const char* lvc, char*** files);
    LVC_API LvcError lvc_status(const char* lvc, char*** files);
    LVC_API LvcError lvc_prepare(const char* lvc, int argc, char* argv[]);
    LVC_API LvcError lvc_version(const char* lvc, const char* message);
    LVC_API LvcError lvc_conflict_manual(const char* lvc, LvcConflictArray* conflicts);
    LVC_API LvcError lvc_conflict_manual_verify(const char* lvc, LvcConflictArray* conflicts);
    LVC_API LvcError lvc_conflict_manual_sync(const char* lvc, LvcConflictArray* conflicts);
    LVC_API LvcError lvc_sync(const char* lvc, const char* tmp_lvc, LvcConflictArray* conflicts);
    LVC_API LvcError lvc_unite(const char* lvc, const char* workspace_name, LvcConflictArray* conflicts);
    LVC_API LvcError lvc_insert(const char* lvc, const char* workspace_name, LvcConflictArray* conflicts);
    LVC_API LvcError lvc_revert(const char* lvc, uint32_t version_id, uint32_t file_count, char** files);
    LVC_API LvcError lvc_push_local(const char* lvc, const char* tmp_lvc);
    LVC_API LvcError lvc_push_server(const char* lvc);
    LVC_API const char* lvc_error_string(LvcError error_code);
    LVC_API LvcBool lvc_category_exists(const char* lvc, const char* name);
    LVC_API LvcBool lvc_workspace_exists(const char* lvc, const char* name);
#ifdef __cplusplus
}
#endif