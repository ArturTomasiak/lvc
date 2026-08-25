#pragma once
#include <lvc.hpp>
#include <helper.hpp>
#include <algorithm>
#include <unordered_set>
#include <unordered_map>

enum ObjectType {
    BLOB,
    TREE
};

struct Object {
    ObjectType type;
    std::string id;
    std::filesystem::path path;
};

namespace category {
    void create(std::filesystem::path workspace_dir, std::string name, char** error_message);
    void rename(const std::filesystem::path& working_dir, const char* category_name, const char* new_name, char** error_message);
    bool exists(std::filesystem::path workspace_dir, std::string name);
}

namespace object {
    void create(const std::filesystem::path& object_dir, const std::string& type, std::string& content, std::string& out_id, char** error_message);
    bool exists (std::filesystem::path object_dir, char id[65]);
}

namespace repository {
    void create(std::filesystem::path lvc, char** error_message);
    void clone(std::filesystem::path working_dir, std::string path, bool clone_versioning, char** error_message);
    void rename(std::filesystem::path lvc, const char* name, char** error_message);
    void storage_template(std::filesystem::path directory_root, StorageBehaviour option, char** error_message);

    LvcConflictArray sync(const std::filesystem::path src_repository, const std::filesystem::path dest_repository, char** error_message);
}

namespace version {
    void create(std::filesystem::path lvc, std::string message, std::string author, std::string inserted_workspace, char** error_message);
    void prepare(std::filesystem::path lvc, std::vector<std::string> input, char*** prepared, char** error_message);
    void prepare_reset(std::filesystem::path lvc, char** error_message);
    void revert(std::filesystem::path lvc, std::string version_id, std::vector<std::string>& input_raw, char** error_message);

    std::vector<std::string> diff(std::filesystem::path lvc, char** error_message);
    std::vector<std::string> status(std::filesystem::path& lvc, std::string& latest_version, char** error_message);
    std::vector<std::string> status_all(std::filesystem::path lvc, char** error_message);

    LvcVersion* history(const std::filesystem::path object_dir, const std::filesystem::path workspace_dir, const std::string workspace_name, size_t depth, size_t length, char** error_message);
    LvcVersion* history_all(const std::filesystem::path object_dir, const std::filesystem::path workspace_dir, const std::string workspace_name, char** error_message);
    void history_free(LvcVersion* version);
    
    std::vector<Object> all_objects(std::filesystem::path object_dir, std::string id, std::unordered_set<std::string_view> ignore, char** error_message);
    std::vector<std::filesystem::path> deleted_since(std::filesystem::path object_dir, std::filesystem::path working_directory, std::string id, char** error_message);
}

namespace workspace {
    void create(std::filesystem::path workspace_dir, std::string category_name, std::string workspace_name, char** error_message);
    bool exists(std::filesystem::path workspace_dir, std::string name, char** error_message);
    bool exists(std::filesystem::path workspace_dir, std::string name, std::string& path, char** error_message);
    bool is_inactive(const std::filesystem::path& workspace_dir, std::string name);
    bool is_inactive(std::filesystem::path workspace);
    void _goto(std::filesystem::path lvc, const char* workspace_name, char** error_message);
    void _default(std::filesystem::path lvc, const char* workspace_name, char** error_message);

    void move_categories(std::filesystem::path workspace_dir, const char* workspace_name, const char* previous_category, const char* category, char** error_message);
    void activate(std::filesystem::path workspace_dir, const char* workspace_name, const char* category_name, char** error_message);
    void deactivate(std::filesystem::path workspace_dir, const char* workspace_name, char** error_message);

    void insert(const std::filesystem::path& lvc, const char* src_workspace, const char* dest_workspace, const char* author, char** error_message);
    void unite(const std::filesystem::path& lvc, const char* src_workspace, const char* dest_workspace, const char* author, char** error_message);
    void push(const std::filesystem::path& src_repository, const std::filesystem::path& dest_repository, char** error_message);

    std::vector<Object> all_objects(std::filesystem::path working_directory, char** error_message);
}

inline void free_charpp(char** arr) {
    if (!arr)
        return;
    for (size_t i = 0; arr[i]; i++)
        free(arr[i]);
    free(arr);
}

inline char** strvector_to_charpp(const std::vector<std::string>& vector) {
    char** result = (char**)calloc((vector.size() + 1), sizeof(char*));
    if (result) {
        char** copy = result;
        for (std::string_view str : vector) {
            *copy = (char*)malloc(str.size() + 1);
            if (!copy) {
                free_charpp(result);
                return 0;
            }
            memcpy(*copy, str.data(), str.size());
            (*copy)[str.size()] = '\0';
            copy++;
        }
        *copy = 0;
    }
    return result;
}

inline size_t charplen(const char* str) {
    size_t len = 0;
    while (*str++)
        len++;
    return len;
}

inline bool charpcmp(const char* str1, const char* str2) {
    while (*str1 && *str2)
        if (*str1++ != *str2++)
            return 0;
    return *str1 == *str2;
}

inline constexpr char DEFAULT_HYBRID_STORAGE[] = R"LVC(# Format: <size in MB> <extension>

# Compressed raster and web graphics
20 jpg
20 jpeg
20 png
20 gif
20 webp
20 avif
20 heic
20 heif

# Layered design, publishing, and high-dynamic-range images
25 psd
25 psb
25 xcf
25 kra
25 tif
25 tiff
25 exr
25 hdr
25 pdf
25 ai
25 eps

# Camera RAW formats
25 dng
25 cr2
25 cr3
25 nef
25 arw
25 raf
25 orf
25 rw2

# Native and proprietary CAD / 3D project formats
20 dwg
20 dgn
20 3dm
20 skp
20 fcstd
20 blend
20 3mf
20 sldprt
20 sldasm
20 catpart
20 catproduct
20 ipt
20 iam
20 prt
20 x_t
20 x_b
20 jt

# CAD and mesh interchange formats
25 dxf
25 step
25 stp
25 iges
25 igs
25 stl
25 obj
25 ply
25 ifc
)LVC";

inline constexpr size_t DEFAULT_HYBRID_STORAGE_LENGTH = sizeof(DEFAULT_HYBRID_STORAGE) - 1;