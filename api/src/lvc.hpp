#pragma once

#include <stdint.h>
#define LvcBool int8_t

#if defined(LVC_STATIC)
    #define LVC_API
#elif defined(_WIN32)
    #if defined(LVC_BUILD_DLL)
        #define LVC_API __declspec(dllexport)
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
    typedef enum StorageBehaviour {
        DISTRIBUTED,
        HYBRID,
        CENTRALIZED
    } StorageBehaviour;
    
    typedef struct LvcCreateInput {
        LvcBool local;
        LvcBool clone;
        StorageBehaviour storage_behaviour;
        char* server_link;
        char* category_name;
        char* workspace_name;
        char* clone_repository;
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

    LVC_API LvcBool lvc_create(const char* lvc, LvcCreateInput input);
    LVC_API LvcBool lvc_workspace(const char* lvc, const char* category_name, const char* workspace_name);
    LVC_API LvcBool lvc_category(const char* lvc, const char* category_name);
    LVC_API LvcBool lvc_goto(const char* lvc, const char* workspace_name);
    LVC_API LvcBool lvc_diff(const char* lvc, char*** files);
    LVC_API LvcBool lvc_status(const char* lvc, char*** files);
    LVC_API LvcBool lvc_prepare(const char* lvc, int argc, char* argv[]);
    LVC_API LvcBool lvc_version(const char* lvc, const char* message);
    LVC_API LvcBool lvc_conflict_manual(const char* lvc, LvcConflictArray* conflicts);
    LVC_API LvcBool lvc_conflict_manual_verify(const char* lvc, LvcConflictArray* conflicts);
    LVC_API LvcBool lvc_conflict_manual_sync(const char* lvc, LvcConflictArray* conflicts);
    LVC_API LvcBool lvc_sync(const char* lvc, const char* tmp_lvc, LvcConflictArray* conflicts);
    LVC_API LvcBool lvc_unite(const char* lvc, const char* workspace_name, LvcConflictArray* conflicts);
    LVC_API LvcBool lvc_insert(const char* lvc, const char* workspace_name, LvcConflictArray* conflicts);
    LVC_API LvcBool lvc_revert(const char* lvc, uint32_t version_id, uint32_t file_count, char** files);
#ifdef __cplusplus
}
#endif