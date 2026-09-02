#pragma once
#include <cstring>
#include <filesystem>
#include <fstream>
#include <lvc.hpp>
#include <vector>
#include <zlib.h>

// as of current unused defines,
// I will eventually rewrite io.cpp to not use c++ abstractions for
// reading/writing
#if defined(_WIN32)
#define WINDOWS
#elif defined(__linux__)
#define LINUX
#elif defined(__APPLE__) && defined(__MACH__)
#define MAC
#endif

void  sha256(const char* in, size_t in_len, char out[65]);
char* deflate(const char* in, size_t in_len, size_t& out_len);
char* inflate(const std::string& in, size_t& out_len);

std::vector<std::string> path_from_input(
    std::filesystem::path repository_root, const std::vector<std::string>& inputs, const std::string version_id, bool prefix_deleted,
    char** error_message);
void insert_pattern(std::string& dest, const std::string& type);

std::filesystem::path workspace_path(const std::filesystem::path& workspace_dir, const std::string& workspace_name);

std::string sync_file(std::string a, std::string b, std::string latest_common, LvcConflictArray conflict, char** error_message);

namespace io {
    void file(std::filesystem::path path, std::ios_base::openmode flags, char** error_message);
    void file(std::filesystem::path path, std::ios_base::openmode flags, const char* content, size_t length, bool compress, char** error_message);
    void prefix_file_content(std::filesystem::path path, const char* content, size_t length, char** error_message);
    bool dir(std::filesystem::path path, char** error_message);
    std::string              content(std::filesystem::path file_path, bool decompress, char** error_message);
    std::string              content_first_line(std::filesystem::path file_path, char** error_message);
    std::vector<std::string> content_lines(std::filesystem::path file_path, bool decompress, char** error_message);
}

inline std::string latest_common_version(const std::vector<std::string>& a, const std::vector<std::string>& b) {
    for (size_t i = 0; i < a.size(); i++)
        for (size_t j = 0; j < b.size(); j++)
            if (a[i] == b[j])
                return a[i];
    return "";
}

inline void error_message_creator(std::string message, char** dest) {
    *dest = (char*)malloc(message.size() + 1);
    memcpy(*dest, message.data(), message.size());
    (*dest)[message.size()] = '\0';
}

inline void error_message_creator_path(std::string message, const std::filesystem::path& path, char** dest) {
    message = message + ' ' + path.string();
    *dest   = (char*)malloc(message.size() + 1);
    memcpy(*dest, message.data(), message.size());
    (*dest)[message.size()] = '\0';
}