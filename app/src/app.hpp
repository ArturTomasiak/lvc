#pragma once

#include <string>
#include <iostream>
#include <cstring>
#include <lvc.hpp>
#include <cctype>
#include <filesystem>

extern std::string error_message;

#define LVCEXEC(function, result, err)   \
do {                                            \
    result = (function);                        \
    if (err) {                                  \
        error_message = lvc_error_string(err);  \
        return 0;                               \
    }                                           \
} while (0)

namespace helper {
    bool find_repository(std::filesystem::path& out);
}

namespace lvc {
    bool create(int argc, char* argv[]);
    bool version(int argc, char* argv[]);
    bool history(int argc, char* argv[]);
    bool prepare(int argc, char* argv[]);
    bool diff(int argc, char* argv[]);
    bool status(int argc, char* argv[]);
    bool _default(int argc, char* argv[]);
    bool _goto(int argc, char* argv[]);
    bool move(int argc, char* argv[]);
    bool revert(int argc, char* argv[]);
}