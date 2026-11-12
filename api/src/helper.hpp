#pragma once
#include <cstring>
#include <filesystem>
#include <fstream>
#include <lvc.hpp>
#include <vector>

struct Paths;

#define CHECKALLOC(dest, error_message, ...)                                                                           \
    do {                                                                                                               \
        if ((dest) == nullptr) {                                                                                       \
            error_message_creator("Memory allocation failed", error_message);                                          \
            return __VA_ARGS__;                                                                                        \
        }                                                                                                              \
    } while (false)

#define CHECKERROR(error_message, ...)                                                                                 \
    if (*error_message) [[unlikely]]                                                                                   \
    return __VA_ARGS__

void  sha256(const char* in, size_t in_len, char out[65]);
char* deflate(const char* in, size_t in_len, size_t& out_len);
char* inflate(const std::string& in, size_t& out_len);

void insert_pattern(std::string& dest, const std::string& type, const char* category);

namespace io {
    void file(const std::filesystem::path& path, std::ios_base::openmode flags, char** error_message);
    void file(
        const std::filesystem::path& path, std::ios_base::openmode flags, const char* content, size_t length,
        bool compress, char** error_message);
    std::string content(const std::filesystem::path& path, bool decompress, bool ignore_pattern, char** error_message);
    std::vector<std::string>
    content_lines(const std::filesystem::path& path, bool decompress, bool ignore_pattern, char** error_message);
    std::vector<std::string> stream_to_lines(std::istream& stream);
}

inline bool valid_name(std::string& name) {
    if (name.empty() || name == "." || name == "..")
        return false;

    if (name.find_first_of("/\\\0\n") != std::string_view::npos)
        return false;

    return true;
}

inline void error_message_creator(const std::string message, char** dest) {
    if (*dest)
        return;
    *dest = static_cast<char*>(malloc(message.size() + 1));
    memcpy(*dest, message.c_str(), message.size() + 1);
}