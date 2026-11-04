#include <paths.hpp>

static std::filesystem::path tmp_base_directory(const std::filesystem::path& root, const std::filesystem::path& local) {
    return std::filesystem::exists(local) ? local / foldername_tmp : root;
}

static std::filesystem::path tmp_directory(const std::filesystem::path& tmp_base, char** error_message) {
    if (!std::filesystem::exists(tmp_base))
        std::filesystem::create_directory(tmp_base);
    std::random_device                           rng;
    std::uniform_int_distribution<std::uint32_t> dist(0, std::numeric_limits<std::uint32_t>::max());
    for (uint8_t i = 0; i < 100; i++) {
        const std::filesystem::path tmp = tmp_base / ("lvc-tmp-" + std::to_string(dist(rng)));
        if (std::filesystem::create_directory(tmp))
            return tmp;
    }
    error_message_creator("Failed to create tmp directory", error_message);
    return std::filesystem::path{};
}

Paths::Paths(const char* lvc_c_str, char** error_message)
        : error_message(error_message),
          lvc(lvc_c_str),
          root(lvc.parent_path()),
          object(lvc / foldername_object),
          workspace(lvc / foldername_workspace),
          current(lvc / filename_current),
          storage(root / filename_storage),
          ignore(root / filename_ignore),
          local(workspace / foldername_local),
          name(lvc / filename_name),
          tmp_base(tmp_base_directory(root, local)),
          tmp(tmp_directory(tmp_base, error_message)) {
        pending.reserve(PREALLOCATE);
    };

    Paths::~Paths() {
        if (*error_message) [[unlikely]] {
            std::filesystem::remove_all(tmp);
            return;
        }

        for (Pending& entry : pending)
            handle_pending(entry);

        std::filesystem::remove_all(tmp);

#ifdef LVC_LINUX
        sync();
#endif
    };