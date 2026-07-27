#pragma once

#include <string>
#include <iostream>
#include <lvc.hpp>
#include <cctype>
#include <filesystem>

extern std::string error_message;

namespace helper {
    bool find_repository(std::filesystem::path& out);
}

namespace lvc {
    bool create(int argc, char* argv[]);
    bool version(int argc, char* argv[]);
    bool prepare(int argc, char* argv[]);
    bool diff(int argc, char* argv[]);
    bool status(int argc, char* argv[]);
    bool _default(int argc, char* argv[]);
    bool _goto(int argc, char* argv[]);
    bool move(int argc, char* argv[]);
}