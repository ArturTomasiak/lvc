#include <helper.hpp>
#include <paths.hpp>

#define CHECKEC(error_message)                                                                                         \
    if (ec) [[unlikely]] {                                                                                             \
        error_message_creator(ec.message(), error_message);                                                            \
        return;                                                                                                        \
    }

void Paths::write_pending(char** error_message) {
    std::error_code ec;
    for (ptrdiff_t i = pending_remove.size() - 1; i >= 0; i--) {
        std::filesystem::remove(pending_remove[i], ec);
        if (ec == std::errc::directory_not_empty)
            continue;
        CHECKEC(error_message);
    }
    for (std::filesystem::path& path : pending_remove_all) {
        std::filesystem::remove_all(path, ec);
        CHECKEC(error_message);
    }
    for (std::filesystem::path& path : pending_directory) {
        std::filesystem::create_directories(path, ec);
        CHECKEC(error_message);
    }
    for (PendingRename& pending : pending_rename) {
        std::filesystem::rename(pending.tmp, pending.dest, ec); // may fail on windows
        CHECKEC(error_message);
    }
    pending_remove.clear();
    pending_remove_all.clear();
    pending_directory.clear();
    pending_rename.clear();
}