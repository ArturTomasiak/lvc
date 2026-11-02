#pragma once

#include <filesystem>
#include <lvc.hpp>
#include <random>
#include <vector>

inline constexpr std::string workspacename_inactive = "inactive";
inline constexpr std::string foldername_workspace   = "workspace";
inline constexpr std::string foldername_local       = "local";
inline constexpr std::string foldername_object      = "object";
inline constexpr std::string filename_default       = "default";
inline constexpr std::string filename_current       = "current";
inline constexpr std::string filename_prepare       = "prepare";
inline constexpr std::string filename_name          = "name";
inline constexpr std::string filename_ignore        = "lvc.ignore";
inline constexpr std::string filename_storage       = "lvc.storage";
inline constexpr std::string filename_tmp           = "tmp";
inline constexpr std::string filename_sync          = "sync";
inline constexpr std::string filename_merge         = "merge";
inline constexpr std::string filename_conflict      = "conflict";

inline std::filesystem::path tmp_directory() {
    std::random_device                           rng;
    std::uniform_int_distribution<std::uint32_t> dist(0, std::numeric_limits<std::uint32_t>::max());
    std::filesystem::path                        tmp;
    do {
        tmp = std::filesystem::temp_directory_path() / ("lvc-" + std::to_string(dist(rng)));
    } while (!std::filesystem::create_directory(tmp));
    return tmp;
}

struct Paths {
    const std::filesystem::path lvc;
    const std::filesystem::path root;
    const std::filesystem::path object;
    const std::filesystem::path workspace;
    const std::filesystem::path current;
    const std::filesystem::path storage;
    const std::filesystem::path ignore;
    const std::filesystem::path local;
    const std::filesystem::path tmp;
    Paths(const char* lvc_c_str)
        : lvc(lvc_c_str),
          root(lvc.parent_path()),
          object(lvc / foldername_object),
          workspace(lvc / foldername_workspace),
          current(lvc / filename_current),
          storage(root / filename_storage),
          ignore(root / filename_ignore),
          local(workspace / foldername_local),
          tmp(tmp_directory()) {};
    std::vector<std::string> resolve(
        const std::vector<std::string>& inputs, const std::string version_id, bool prefix_deleted, bool include_folders,
        char** error_message);
};