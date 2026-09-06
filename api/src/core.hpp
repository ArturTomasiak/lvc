#pragma once
#include <algorithm>
#include <helper.hpp>
#include <lvc.hpp>
#include <unordered_map>
#include <unordered_set>

struct Paths {
    const std::filesystem::path lvc;
    const std::filesystem::path root;
    const std::filesystem::path object;
    const std::filesystem::path workspace;
    const std::filesystem::path current;
    const std::filesystem::path storage;
    const std::filesystem::path ignore;
    const std::filesystem::path local;
    Paths(const char* lvc_c_str)
        : lvc(lvc_c_str),
          root(lvc.parent_path()),
          object(lvc / NAME_OBJECT),
          workspace(lvc),
          current(lvc / NAME_CURRENT),
          storage(root / NAME_STORAGE),
          ignore(root / NAME_IGNORE),
          local(workspace / NAME_LOCAL) {};
    std::vector<std::string> from_input(
        const std::vector<std::string>& inputs, const std::string version_id, bool prefix_deleted,
        char** error_message);
};

namespace category {
    void create(Paths& paths, std::string name, char** error_message);
    void rename(Paths& paths, const char* category_name, const char* new_name, char** error_message);
    bool exists(Paths& paths, std::string name);
}

namespace operation {
    inline constexpr char filename_sync[]   = "sync";
    inline constexpr char filename_unite[]  = "unite";
    inline constexpr char filename_insert[] = "insert";

    enum class type : uint8_t { none, sync, unite, insert };

    void create_folder(const std::filesystem::path& operation, operation::type type_enum, char** error_message);
    operation::type  ongoing(Paths& paths, std::string& workspace_name, char** error_message);
    LvcConflictArray sync(Paths& src_paths, Paths& dest_paths, char** error_message);
    void             insert(
                    Paths& paths, std::string src_workspace, std::string dest_workspace, const char* author, char** error_message);
    void unite(
        Paths& paths, std::string src_workspace, std::string dest_workspace, const char* author, char** error_message);
    void push(Paths& src_paths, Paths& dest_paths, char** error_message);
}

namespace object {
    enum class type : uint8_t { blob, tree };

    struct info {
        object::type          type;
        std::string           id;
        std::filesystem::path path;
    };

    void create(
        const std::filesystem::path& object, const std::string& type, std::string& content, std::string& out_id,
        char** error_message);
    bool exists(Paths& paths, char id[65]);
}

namespace repository {
    void create(Paths& paths, char** error_message);
    void clone(Paths& paths, std::string path, bool clone_versioning, char** error_message);
    void rename(Paths& paths, const char* name, char** error_message);
    void storage_template(Paths& paths, StorageBehaviour option, char** error_message);
}

namespace version {
    void
    create(Paths& paths, std::string message, std::string author, std::string inserted_workspace, char** error_message);
    void create_tmp(Paths& paths, std::filesystem::path& operation, char** error_message);
    void prepare(Paths& paths, std::vector<std::string> input, char*** prepared, char** error_message);
    void prepare_reset(Paths& paths, char** error_message);
    void revert(Paths& paths, std::string version_id, std::vector<std::string>& input_raw, char** error_message);

    std::vector<std::string> checkout(Paths& paths, char** error_message);
    std::vector<std::string> status(Paths& paths, std::string& latest_version, char** error_message);
    std::vector<std::string> status_all(Paths& paths, char** error_message);

    LvcVersion*
    history(Paths& paths, const std::string workspace_name, size_t depth, size_t length, char** error_message);
    LvcVersion* history_all(Paths& paths, const std::string workspace_name, char** error_message);
    void        history_free(LvcVersion* version);

    std::vector<object::info> all_objects(
        const std::filesystem::path& object, const std::string& id, std::unordered_set<std::string_view> ignore,
        char** error_message);
    std::unordered_map<std::filesystem::path, object::info> map_all_objects(
        Paths& paths, const std::string& id, std::unordered_set<std::string_view> ignore, char** error_message);
    std::vector<std::filesystem::path> deleted_since(Paths& paths, std::string id, char** error_message);
}

namespace workspace {
    void create(
        Paths& paths, std::string category_name, std::string workspace_name, bool clone_working, char** error_message);
    bool exists(Paths& paths, std::string name, char** error_message);
    bool exists(Paths& paths, std::string name, std::string& path, char** error_message);
    bool is_inactive(Paths& paths, std::string name);
    bool is_inactive(const std::filesystem::path& workspace);
    void _goto(Paths& paths, std::string workspace_name, char** error_message);
    void _default(Paths& paths, const char* workspace_name, char** error_message);

    void move_categories(
        Paths& paths, const char* workspace_name, const char* previous_category, const char* category,
        char** error_message);
    void activate(Paths& paths, const char* workspace_name, const char* category_name, char** error_message);
    void deactivate(Paths& paths, const char* workspace_name, char** error_message);

    std::vector<object::info> all_objects(Paths& paths, char** error_message);
}

inline void free_charpp(char** arr) {
    if (!arr)
        return;
    for (size_t i = 0; arr[i]; i++)
        free(arr[i]);
    free(arr);
}

inline char** strvector_to_charpp(const std::vector<std::string>& vector) {
    char** result = (char**)calloc((vector.size() + 1), sizeof(char*));
    if (result) {
        char** copy = result;
        for (std::string_view str : vector) {
            *copy = (char*)malloc(str.size() + 1);
            if (!copy) {
                free_charpp(result);
                return 0;
            }
            memcpy(*copy, str.data(), str.size());
            (*copy)[str.size()] = '\0';
            copy++;
        }
        *copy = 0;
    }
    return result;
}

inline size_t charplen(const char* str) {
    size_t len = 0;
    while (*str++)
        len++;
    return len;
}

inline bool charpcmp(const char* str1, const char* str2) {
    while (*str1 && *str2)
        if (*str1++ != *str2++)
            return 0;
    return *str1 == *str2;
}

inline constexpr char DEFAULT_HYBRID_STORAGE[] =
    R"LVC(# Format: <size in MB> <extension>

# Compressed raster and web graphics
20 jpg
20 jpeg
20 png
20 gif
20 webp
20 avif
20 heic
20 heif

# Layered design, publishing, and high-dynamic-range images
25 psd
25 psb
25 xcf
25 kra
25 tif
25 tiff
25 exr
25 hdr
25 pdf
25 ai
25 eps

# Camera RAW formats
25 dng
25 cr2
25 cr3
25 nef
25 arw
25 raf
25 orf
25 rw2

# Native and proprietary CAD / 3D project formats
20 dwg
20 dgn
20 3dm
20 skp
20 fcstd
20 blend
20 3mf
20 sldprt
20 sldasm
20 catpart
20 catproduct
20 ipt
20 iam
20 prt
20 x_t
20 x_b
20 jt

# CAD and mesh interchange formats
25 dxf
25 step
25 stp
25 iges
25 igs
25 stl
25 obj
25 ply
25 ifc
)LVC";

inline constexpr size_t DEFAULT_HYBRID_STORAGE_LENGTH = sizeof(DEFAULT_HYBRID_STORAGE) - 1;