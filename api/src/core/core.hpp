#pragma once
#include <lvc.hpp>
#include <zlib.h>
#include <helper.hpp>
#include <filesystem>
#include <fstream>
#include <cstring>
#include <algorithm>
#include <unordered_set>
#include <vector>

#define RETURN_ERR LVC_RETURN_IF_ERROR

#ifdef TEST_PRINTS
#include <iostream>
#endif

inline constexpr char EMPTY_SHA256[65] = "e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855";

// defines for which line of deflated version has what content
#define VERSION_TYPE      0
#define VERSION_ROOT_TREE 1
#define VERSION_AUTHOR    2
#define VERSION_WORKSPACE 3

// file/folder name defines 
#define NAME_WORKSPACE "workspace"
#define NAME_OBJECT "object"
#define NAME_STATUS "status"
#define NAME_DEFAULT "default"
#define NAME_CURRENT "current"
#define NAME_PREPARE "prepare"
#define NAME_NAME "name"
#define NAME_STORAGE "lvc.storage"

// type string defines
#define TYPE_TREE "tree"
#define TYPE_BLOB "blob"

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
    bool exists(std::filesystem::path workspace_dir, std::string name);
}

namespace file {
    bool create(std::filesystem::path path, std::ios_base::openmode flags);
    bool create(std::filesystem::path path, std::ios_base::openmode flags, const char* content, uint64_t length, bool deflate);
    bool create_dir(std::filesystem::path lvc);

    std::string content(std::filesystem::path file_path, bool deflated);
    std::string content_first_line(std::filesystem::path file_path);
    std::vector<std::string> content_lines(std::filesystem::path file_path, bool deflated);

    std::vector<std::string> path_from_input(std::filesystem::path repository_root, const std::vector<std::string>& inputs);
}

namespace object {
    LvcError create(const std::filesystem::path& object_dir, const std::string& type, std::string& content, char id[65]);
    void     insert_pattern(std::string& content, const std::string& type);
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
    LvcError prepare(std::filesystem::path lvc, std::vector<std::string> input);
    LvcError revert(std::filesystem::path lvc, uint32_t version_id, std::vector<std::string> input);

    std::vector<std::string> diff(std::filesystem::path lvc);
    std::vector<std::string> status(std::filesystem::path lvc);
    std::vector<std::string> status_all(std::filesystem::path lvc);
    
    std::string latest(std::filesystem::path branch);
    std::vector<Object> all_objects(std::filesystem::path object, std::filesystem::path working_directory, std::string id);
    std::vector<std::filesystem::path> deleted_since(std::filesystem::path lvc, std::string id);
}

namespace workspace {
    LvcError create(std::filesystem::path workspace_dir, std::string category_name, std::string workspace_name);
    bool exists(std::filesystem::path workspace_dir, std::string name);
    bool exists(std::filesystem::path workspace_dir, std::string name, std::string& path);
    LvcError _goto(std::filesystem::path lvc, const char* category_name, const char* workspace_name);
    LvcError _default(std::filesystem::path lvc, const char* category_name, const char* workspace_name);

    LvcError insert(const std::filesystem::path& lvc, const std::filesystem::path& src_workspace, const std::filesystem::path& dest_workspace);
    LvcError unite(const std::filesystem::path& lvc, const std::filesystem::path& src_workspace, const std::filesystem::path& dest_workspace);
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

inline char* charpcombslash(const char* str1, const char* str2) {
    uint64_t str1len = charplen(str1);
    uint64_t str2len = charplen(str2);
    char* result = (char*)malloc(str1len + str2len + 2);
    if (result) {
        memcpy(result, str1, str1len);
        result[str1len] = '/';
        memcpy(result + str1len + 1, str2, str2len);
        result[str1len + str2len + 1] = '\0';
    }
    return result;
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