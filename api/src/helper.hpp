#pragma once
#include <cstring>
#include <filesystem>
#include <fstream>
#include <lvc.hpp>
#include <paths.hpp>
#include <vector>

#define CHECKALLOC(dest, error_message, ...)                                                                           \
    do {                                                                                                               \
        if ((dest) == nullptr) {                                                                                       \
            error_message_creator("Memory allocation failed", error_message);                                          \
            return __VA_ARGS__;                                                                                        \
        }                                                                                                              \
    } while (false)

void  sha256(const char* in, size_t in_len, char out[65]);
char* deflate(const char* in, size_t in_len, size_t& out_len);
char* inflate(const std::string& in, size_t& out_len);

void                  insert_pattern(std::string& dest, const std::string& type, const char* category);
std::filesystem::path prepare_path(Paths& paths, const std::string& workspace_name);

namespace io {
    void file(std::filesystem::path path, std::ios_base::openmode flags, char** error_message);
    void file(
        std::filesystem::path path, std::ios_base::openmode flags, const char* content, size_t length, bool compress,
        char** error_message);
    bool        dir(std::filesystem::path path, char** error_message);
    std::string content(std::filesystem::path file_path, bool decompress, bool ignore_pattern, char** error_message);
    std::vector<std::string>
    content_lines(std::filesystem::path file_path, bool decompress, bool ignore_pattern, char** error_message);
}

inline std::string latest_common_version(const std::vector<std::string>& a, const std::vector<std::string>& b) {
    for (size_t i = 0; i < a.size(); i++)
        for (size_t j = 0; j < b.size(); j++)
            if (a[i] == b[j])
                return a[i];
    return "";
}

inline void error_message_creator(const std::string message, char** dest) {
    if (*dest)
        return;
    *dest = static_cast<char*>(malloc(message.size() + 1));
    memcpy(*dest, message.c_str(), message.size() + 1);
}