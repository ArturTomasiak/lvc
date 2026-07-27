#pragma once
#include <lvc.hpp>
#include <helper.hpp>
#include <algorithm>
#include <unordered_set>

#define RETURN_ERR LVC_RETURN_IF_ERROR

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
    LvcError create(std::filesystem::path workspace_dir, std::string name);
    LvcError rename(const std::filesystem::path& working_dir, const char* category_name, const char* new_nam);
    bool exists(std::filesystem::path workspace_dir, std::string name);
}

namespace object {
    LvcError create(const std::filesystem::path& object_dir, const std::string& type, std::string& content, char id[65]);
    bool     exists (std::filesystem::path object_dir, char id[65]);
    char*    deflate(const char* in, uint64_t in_len, uint64_t& out_len);
    char*    inflate(const std::string& in, uint64_t& out_len);
}

namespace repository {
    LvcError create(std::filesystem::path lvc);
    LvcError rename(std::filesystem::path lvc, const char* name);
    LvcError storage_template(std::filesystem::path directory_root, StorageBehaviour option);
}

namespace version {
    LvcError create(std::filesystem::path lvc, const char* message);
    LvcError prepare(std::filesystem::path lvc, std::vector<std::string> input, char*** prepared);
    LvcError revert(std::filesystem::path lvc, uint32_t version_id, std::vector<std::string> input);

    std::vector<std::string> diff(std::filesystem::path lvc);
    std::vector<std::string> status(std::filesystem::path lvc);
    std::vector<std::string> status_all(std::filesystem::path lvc);
    
    std::string latest(std::filesystem::path branch);
    std::vector<Object> all_objects(std::filesystem::path object_dir, std::filesystem::path working_directory, std::string id);
    std::vector<Object> deleted_since(std::filesystem::path object_dir, std::filesystem::path working_directory, std::string id);
}

namespace workspace {
    LvcError create(std::filesystem::path workspace_dir, std::string category_name, std::string workspace_name);
    bool exists(std::filesystem::path workspace_dir, std::string name);
    bool exists(std::filesystem::path workspace_dir, std::string name, std::string& path);
    bool is_inactive(const std::filesystem::path& workspace_dir, std::string name);
    bool is_inactive(std::filesystem::path workspace);
    LvcError _goto(std::filesystem::path lvc, const char* workspace_name);
    LvcError _default(std::filesystem::path lvc, const char* workspace_name);

    LvcError move_categories(std::filesystem::path workspace_dir, const char* workspace_name, const char* previous_category, const char* category);
    LvcError activate(std::filesystem::path workspace_dir, const char* workspace_name, const char* category_name);
    LvcError deactivate(std::filesystem::path workspace_dir, const char* workspace_name);
    LvcError insert(const std::filesystem::path& lvc, const std::filesystem::path& src_workspace, const std::filesystem::path& dest_workspace);
    LvcError unite(const std::filesystem::path& lvc, const std::filesystem::path& src_workspace, const std::filesystem::path& dest_workspace);
    

    std::vector<Object> all_objects(std::filesystem::path working_directory);
}

inline void free_charpp(char** arr) {
    if (arr == 0)
        return;
    for (int i = 0; arr[i] != 0; i++)
        free(arr[i]);
    free(arr);
}

inline char** strvector_to_charpp(const std::vector<std::string>& vector) {
    char** result = (char**)calloc((vector.size() + 1), sizeof(char*));
    if (result) {
        char** copy = result;
        for (const std::string& str : vector) {
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

inline uint64_t charplen(const char* str) {
    uint64_t len = 0;
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

inline constexpr uint64_t DEFAULT_HYBRID_STORAGE_LENGTH = sizeof(DEFAULT_HYBRID_STORAGE) - 1;