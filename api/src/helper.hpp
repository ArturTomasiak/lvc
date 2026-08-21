#pragma once
#include <lvc.hpp>
#include <zlib.h>
#include <fstream>
#include <cstring>
#include <vector>
#include <filesystem>

// as of current unused defines,
// I will eventually rewrite io.cpp to not use c++ abstractions for reading/writing
#if defined(_WIN32)
    #define WINDOWS
#elif defined(__linux__)
    #define LINUX
#elif defined(__APPLE__) && defined(__MACH__)
    #define MAC
#endif

void sha256(const char* in, size_t in_len, char out[65]);
char* deflate(const char* in, size_t in_len, size_t& out_len);
char* inflate(const std::string& in, size_t& out_len);

std::vector<std::string> path_from_input(std::filesystem::path repository_root, const std::vector<std::string>& inputs, const std::string version_id, bool prefix_deleted, LvcError& err);
void insert_pattern(std::string& content, const std::string& type);

std::filesystem::path workspace_path(const std::filesystem::path& workspace_dir, const std::string& workspace_name);

std::string sync_file(std::string a, std::string b, std::string latest_common, LvcConflictArray conflict);

namespace io {
    LvcError file(std::filesystem::path path, std::ios_base::openmode flags);
    LvcError file(std::filesystem::path path, std::ios_base::openmode flags, const char* content, size_t length, bool compress);
    LvcError prefix_file_content(std::filesystem::path path, const char* content, size_t length);
    bool dir(std::filesystem::path lvc);
    std::string content(std::filesystem::path file_path, bool decompress);
    std::string content_first_line(std::filesystem::path file_path);
    std::vector<std::string> content_lines(std::filesystem::path file_path, bool decompress);
}
