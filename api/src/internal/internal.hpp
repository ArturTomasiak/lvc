#pragma once
#include <filesystem>
#include <fstream>
#include <lvc.hpp>
#include <cstring>

#define RETURN_ERR LVC_RETURN_IF_ERROR

namespace create {
    LvcError lvc(std::filesystem::path lvc);
    LvcError category(std::filesystem::path lvc, std::string name);
    LvcError workspace(std::filesystem::path lvc, std::string category_name, std::string workspace_name);
    LvcError storage(std::filesystem::path lvc, StorageBehaviour option);
}

namespace write {
    bool file(std::filesystem::path path, std::ios_base::openmode flags, const char* content);
    LvcError workspace_current(std::filesystem::path lvc, const char* workspace_name);
    LvcError workspace_default(std::filesystem::path lvc, const char* workspace_name);
}

namespace exists {
    bool category(std::filesystem::path lvc, std::string name);
    bool workspace(std::filesystem::path lvc, std::string name, std::string* path);
}