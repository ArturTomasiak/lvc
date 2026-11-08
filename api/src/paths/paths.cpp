#include <paths.hpp>

static std::filesystem::path tmp_directory(
    const std::filesystem::path& root, const std::filesystem::path& lvc, const std::filesystem::path& local,
    char** error_message) {
    const std::filesystem::path& base = std::filesystem::is_directory(lvc) ? local / filename_tmp : root;
    if (!std::filesystem::is_directory(base))
        std::filesystem::create_directories(base);
    std::random_device                           rng;
    std::uniform_int_distribution<std::uint32_t> dist(0, std::numeric_limits<std::uint32_t>::max());
    for (uint8_t i = 0; i < 100; i++) {
        const std::filesystem::path tmp = base / ("lvc-tmp-" + std::to_string(dist(rng)));
        if (std::filesystem::create_directory(tmp))
            return tmp;
    }
    error_message_creator("Failed to create tmp directory", error_message);
    return std::filesystem::path{};
}

Paths::Paths(const char* lvc_c_str, bool preallocate, char** error_message)
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
      tmp(tmp_directory(root, lvc, local, error_message)) {
    if (preallocate)
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