#pragma once
#include <filesystem>
#include <fstream>
#include <lvc.hpp>
#include <cstring>
#include <algorithm>
#include <unordered_set>
#include <vector>

#define RETURN_ERR LVC_RETURN_IF_ERROR

#ifdef TEST_PRINTS
#include <iostream>
#endif

namespace create {
    bool dir(std::filesystem::path lvc);

    LvcError lvc(std::filesystem::path lvc);
    LvcError category(std::filesystem::path lvc, std::string name);
    LvcError workspace(std::filesystem::path lvc, std::string category_name, std::string workspace_name);
    LvcError storage(std::filesystem::path lvc, StorageBehaviour option);
}

namespace write {
    bool file(std::filesystem::path path, std::ios_base::openmode flags);
    bool file(std::filesystem::path path, std::ios_base::openmode flags, const char* content, uint64_t length);

    LvcError workspace_current(std::filesystem::path lvc, const char* category_name, const char* workspace_name);
    LvcError workspace_default(std::filesystem::path lvc, const char* category_name, const char* workspace_name);
    LvcError repository_name(std::filesystem::path lvc, const char* name);
    LvcError version(std::filesystem::path lvc, const char* message);
    LvcError prepare(std::filesystem::path lvc, std::vector<std::string> input);
    LvcError revert(std::filesystem::path lvc, uint32_t version_id, std::vector<std::string> input);
}

namespace exists {
    bool category(std::filesystem::path lvc, std::string name);
    bool workspace(std::filesystem::path lvc, std::string name);
    bool workspace(std::filesystem::path lvc, std::string name, std::string& path);
}

namespace get {
    std::vector<std::string> diff(std::filesystem::path lvc);
    std::vector<std::string> status(std::filesystem::path lvc);
    std::vector<std::string> status_all(std::filesystem::path lvc);
    std::string file_content(std::filesystem::path file_path);
    std::vector<std::string> file_content_lines(std::filesystem::path file_path);
    std::vector<std::string> path_from_input(std::filesystem::path repository_root, const std::vector<std::string>& inputs);
    bool sha256(const char* in, uint64_t in_len, char out[65]);
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

inline char* strvector_to_charp(std::vector<std::string> vector) {
    std::erase_if(vector, [](const std::string& str) {
        return str.empty();
    });
    if (vector.empty())
        return 0;
    uint64_t len  = vector.size();
    uint64_t size = 0;
    uint64_t pos  = 0;
    for (const std::string& str : vector) 
        size += str.size() + 1;
    char* result = (char*)malloc(size);
    if (!result)
        return 0;
    for (uint64_t i = 0; i < len; i++) {
        uint64_t slen = vector[i].size();
        memcpy(result + pos, vector[i].c_str(), slen);
        pos += slen;
        if (i + 1 != len)
            result[pos++] = '\n';
    }
    result[pos] = '\0';
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
        result[str2len + 1] = '\0';
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