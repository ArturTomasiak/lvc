#pragma once
#include <string>

struct State {
    std::string lvc_directory = "";
    std::string config_path   = "";
    int file_limit     = 0;
    int oldest_version = 0;
    bool remove_lvc_on_failure = 0;
    void initialize(); // defined in file.cpp
};

extern State state;

inline constexpr const char* default_settings = 
R"(# leave default if not using a centralized server

# the size from which a .lvc file is not stored locally, 0 means no limit
file_limit=0

# version from which .lvc files are stored locally, 0 means all versions are stored locally
oldest_version=0
)";