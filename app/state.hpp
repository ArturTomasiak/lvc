#pragma once
#include <string>
#include <unordered_map>
#include <stdexcept>

#define FLAGS_LVC_INITIALIZED        0b1
#define FLAGS_STORE_TEXT_SERVER      0b10
#define FLAGS_STORE_NON_TEXT_SERVER  0b100
#define FLAGS_REMOVE_LVC_ON_FAILURE  0b1000

struct State {
    int limit_text     = 0;
    int limit_non_text = 0;
    int flags = 0;
    std::string lvc_directory = "";
    std::string config_path   = "";
    std::string server        = "";
    std::string server_key    = "";
// defined in file.cpp
    void find_lvc();
    void read_config();
// read_config helpers
private:
    void store_text_set(int value) {
        if (value)
            flags |= FLAGS_STORE_TEXT_SERVER;
    }
    void store_non_text_set(int value) {
        if (value)
            flags |= FLAGS_STORE_NON_TEXT_SERVER;
    }
    void limit_text_set(int value) {
        limit_text = value;
    }
    void limit_non_text_set(int value) {
        limit_non_text = value;
    }
    std::unordered_map<std::string, void(State::*)(int value)> int_settings_map = {
        {"store_text", &State::store_text_set},
        {"store_non_text", &State::store_non_text_set},
        {"limit_text", &State::limit_text_set},
        {"limit_non_text", &State::limit_non_text_set}
    };
    void match_string_settings(std::string name, std::string string) {
        if (name == "sever")
            server = string;
        else if (name == "server_key")
            server_key = string;
    }
    void match_int_settings(std::string name, std::string string) {
        std::unordered_map<std::string, void (State::*)(int)>::iterator iterator = int_settings_map.find(name);
        if (iterator != int_settings_map.end()) {
            try {
                int value = std::stoi(string);
                (this->*(iterator->second))(value);
            } catch(const std::exception& e) {
                throw std::runtime_error("Invalid value in lvc.config - non integer where number is expected");
            } 
        } 
    } 
};

extern State state;

inline constexpr const char* default_settings = 
R"(#NA = local, <path> = use centralized
server=NA
server_key=NA
#storage settings regarding .lvc blobs
#0 = local, 1 = server
store_text=0
store_non_text=0
#0 = no limit, <num> = size in mb that will make a file stored on server if exceeds
limit_text=0
limit_non_text=0
)";