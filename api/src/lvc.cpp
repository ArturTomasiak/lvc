#include "lvc.hpp"

LVC_API LvcBool lvc_create(const char* lvc, LvcCreateInput input) {
    return 1;
}

LVC_API int8_t lvc_workspace(const char* lvc, const char* category_name, const char* workspace_name) {
    return 1;
}

LVC_API int8_t lvc_category(const char* lvc, const char* category_name) {
    return 1;
}

LVC_API int8_t lvc_goto(const char* lvc, const char* workspace_name) {
    return 1;
}

LVC_API int8_t lvc_diff(const char* lvc, char*** files) {
    return 1;
}

LVC_API int8_t lvc_status(const char* lvc, char*** files) {
    return 1;
}

LVC_API int8_t lvc_prepare(const char* lvc, int argc, char* argv[]) {
    return 1;
}

LVC_API int8_t lvc_version(const char* lvc, const char* message) {
    return 1;
}

LVC_API int8_t lvc_conflict_manual(const char* lvc, LvcConflictArray* conflicts) {
    return 1;
}

LVC_API int8_t lvc_conflict_manual_verify(const char* lvc, LvcConflictArray* conflicts) {
    return 1;
}

LVC_API int8_t lvc_conflict_manual_sync(const char* lvc, LvcConflictArray* conflicts) {
    return 1;
}

LVC_API int8_t lvc_sync(const char* lvc, const char* tmp_lvc, LvcConflictArray* conflicts){
    return 1;
}

LVC_API int8_t lvc_unite(const char* lvc, const char* workspace_name, LvcConflictArray* conflicts) {
    return 1;
}

LVC_API int8_t lvc_insert(const char* lvc, const char* workspace_name, LvcConflictArray* conflicts) {
    return 1;
}

LVC_API int8_t lvc_revert(const char* lvc, uint32_t version_id, uint32_t file_count, char** files) {
    return 1;
}
