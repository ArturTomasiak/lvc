#pragma once
#include <filesystem>
#include <lvc.hpp>

#define RETURN_ERR LVC_RETURN_IF_ERROR

namespace create {
    LvcError lvc(std::filesystem::path lvc_path);
    LvcError category(std::filesystem::path lvc_path, std::string name);
    LvcError workspace(std::filesystem::path lvc_path, std::string name);
    LvcError storage(std::filesystem::path lvc_path, StorageBehaviour option);
}

namespace exists {
    bool category(std::filesystem::path lvc_path, std::string name);
    bool workspace(std::filesystem::path lvc_path, std::string name);
}