#pragma once

#include <filesystem>
#include <helper.hpp>
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

inline std::filesystem::path tmp_directory(char** error_message) {
    std::random_device                           rng;
    std::uniform_int_distribution<std::uint32_t> dist(0, std::numeric_limits<std::uint32_t>::max());
    for (uint8_t i = 0; i < 100; i++) {
        const std::filesystem::path tmp = std::filesystem::temp_directory_path() / ("lvc-" + std::to_string(dist(rng)));
        if (std::filesystem::create_directory(tmp))
            return tmp;
    }
    error_message_creator("Failed to create tmp directory", error_message);
    return std::filesystem::path{};
}

enum class PendingType : uint8_t { remove, remove_all, create_directory, create_directories, rename };

struct Pending {
    const PendingType           type;
    const std::filesystem::path src;
    const std::filesystem::path dest;
};

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

    Paths(const char* lvc_c_str, char** error_message)
        : error_message(error_message),
          lvc(lvc_c_str),
          root(lvc.parent_path()),
          object(lvc / foldername_object),
          workspace(lvc / foldername_workspace),
          current(lvc / filename_current),
          storage(root / filename_storage),
          ignore(root / filename_ignore),
          local(workspace / foldername_local),
          tmp(tmp_directory(error_message)) {
        pending.reserve(PREALLOCATE_SMALL);
    };

    ~Paths() {
        for (Pending& entry : pending)
            handle_pending(entry);
    };

    std::vector<std::string> resolve(
        const std::vector<std::string>& inputs, const std::string version_id, bool prefix_deleted,
        bool include_folders);

  private:
    char**               error_message;
    std::vector<Pending> pending;
    void                 handle_pending(const Pending& pending);
};