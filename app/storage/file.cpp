#include <file.hpp>
#include <filesystem>
#include <fstream>
#include <cstring>
#include <iostream>
#include <stdexcept>
#include <state.hpp>

void create_folder(const char* path) {
    std::error_code error;
    if (!std::filesystem::create_directories(path, error) || error) {
        std::string errmsg = std::string("Failed to create directory ") + path + "\n";
        if (error)
            errmsg += error.message() + "\n";
        else
            errmsg += "Directory already existed.\n";
        throw std::runtime_error(errmsg);
    }
}

void create_file(const char* path, const char* content) {
    std::ofstream file(path, std::ios::binary);
    if (!file)
        throw std::runtime_error("Could not create file.");
    file.write(content, std::strlen(content));
    file.close();
}

void delete_folder(const char* path) {
    std::filesystem::remove_all(path);
}

void delete_file(const char* path) {
    std::filesystem::remove(path);
}

std::string lvc_path() {
    std::filesystem::path start = std::filesystem::current_path();

    while (true) {
        std::filesystem::path lvc_dir = start / ".lvc";

        if (std::filesystem::is_directory(lvc_dir))
            return lvc_dir.string();

        std::filesystem::path parent = start.parent_path();
        if (parent == start)
            return "";
        start = parent;
    }
}

void State::find_lvc() {
    lvc_directory = lvc_path();
    if (lvc_directory.empty())
        return;
    flags |= FLAGS_LVC_INITIALIZED;
    std::filesystem::path lvc = lvc_directory;
    config_path = (lvc.parent_path() / "lvc.config").string();
}

void State::read_config() {
    std::ifstream file(config_path);
    if (!file) {
        create_file(config_path.c_str(), default_settings);
        return;
    }

    std::string line;
    while (std::getline(file, line)) {
        if (line.empty() || line[0] == '#')
            continue;
        std::size_t pos = line.find('=');
        if (pos == std::string::npos)
            continue;
        std::string name = line.substr(0, pos);
        std::string value = line.substr(pos + 1);
        match_string_settings(name, value);
        match_int_settings(name, value);
    }
}