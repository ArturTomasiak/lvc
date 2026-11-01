#pragma once

#include <filesystem>
#include <helper.hpp>
#include <lvc.hpp>
#include <random>
#include <string>
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

struct PendingRename {
    std::filesystem::path tmp;
    std::filesystem::path dest;
};

struct Paths {
    const std::filesystem::path        lvc;
    const std::filesystem::path        root;
    std::filesystem::path              object;
    const std::filesystem::path        workspace;
    const std::filesystem::path        current;
    const std::filesystem::path        storage;
    const std::filesystem::path        ignore;
    const std::filesystem::path        local;
    std::filesystem::path              tmp;
    std::vector<PendingRename>         pending_rename;
    std::vector<std::filesystem::path> pending_directory;
    std::vector<std::filesystem::path> pending_delete;

    Paths(const Paths&)            = delete;
    Paths& operator=(const Paths&) = delete;
    Paths(Paths&&)                 = delete;
    Paths& operator=(Paths&&)      = delete;

    Paths(const char* lvc_c_str, char** error_message)
        : lvc(lvc_c_str),
          root(lvc.parent_path()),
          object(lvc / foldername_object),
          workspace(lvc / foldername_workspace),
          current(lvc / filename_current),
          storage(root / filename_storage),
          ignore(root / filename_ignore),
          local(workspace / foldername_local) {
        pending_rename.reserve(PREALLOCATE_SMALL);
        pending_directory.reserve(PREALLOCATE_SMALL);
        pending_delete.reserve(PREALLOCATE_SMALL);
        std::random_device                           rng;
        std::uniform_int_distribution<std::uint32_t> dist(0, std::numeric_limits<std::uint32_t>::max());
        for (uint8_t i = 0; i < 100; i++) {
            tmp = lvc / "tmp" / std::to_string(dist(rng));
            if (std::filesystem::create_directories(tmp))
                return;
        }
        error_message_creator("Failed to create tmp directory.", error_message);
    };

    ~Paths() {
        std::error_code ec;
        std::filesystem::remove_all(tmp, ec);
    }

    std::vector<std::string> resolve(
        const std::vector<std::string>& inputs, const std::string version_id, bool prefix_deleted, bool include_folders,
        char** error_message);
    void write_pending(char** error_message);

    std::filesystem::path add_pending_rename(const std::filesystem::path& dest) {
        pending_rename.push_back({.tmp = tmp / std::to_string(pending_rename.size()), .dest = dest});
        return pending_rename.back().tmp;
    }
};