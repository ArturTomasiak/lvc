#pragma once

#include <filesystem>
#include <helper.hpp>
#include <lvc.hpp>
#include <random>
#include <unistd.h>
#include <vector>

inline constexpr std::string foldername_workspace = "workspace";
inline constexpr std::string foldername_local     = "local";
inline constexpr std::string foldername_object    = "object";
inline constexpr std::string foldername_tmp       = "tmp";
inline constexpr std::string filename_default     = "default";
inline constexpr std::string filename_current     = "current";
inline constexpr std::string filename_prepare     = "prepare";
inline constexpr std::string filename_name        = "name";
inline constexpr std::string filename_ignore      = "lvc.ignore";
inline constexpr std::string filename_storage     = "lvc.storage";
inline constexpr std::string filename_tmp         = "tmp";
inline constexpr std::string filename_merge       = "merge";
inline constexpr std::string filename_conflict    = "conflict";

enum class PendingType : uint8_t {
    remove,
    remove_conditional,
    remove_all,
    create_directory,
    create_directory_conditional,
    create_directories,
    create_directories_conditional,
    rename
};

struct Pending {
    const PendingType           type;
    const std::filesystem::path src;
    const std::filesystem::path dest;
};

class Paths {
    char**               error_message;
    std::vector<Pending> pending;
    size_t               tmp_counter = 0;

    void handle_pending(const Pending& pending);

  public:
    const std::filesystem::path lvc;
    const std::filesystem::path root;
    const std::filesystem::path object;
    const std::filesystem::path workspace;
    const std::filesystem::path current;
    const std::filesystem::path storage;
    const std::filesystem::path ignore;
    const std::filesystem::path local;
    const std::filesystem::path name;
    const std::filesystem::path tmp;

    Paths(const char* lvc_c_str, bool preallocate, char** error_message);
    ~Paths();
    void                     move_pending(const Pending& entry);
    std::filesystem::path    get_tmp_file();
    std::vector<std::string> resolve(
        const std::vector<std::string>& inputs, const std::string version_id, bool prefix_deleted,
        bool include_folders);
};