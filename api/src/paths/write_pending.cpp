#include <helper.hpp>
#include <paths.hpp>

#define CHECKEC(error_message)                                                                                         \
    if (ec) [[unlikely]] {                                                                                             \
        error_message_creator(ec.message(), error_message);                                                            \
        return;                                                                                                        \
    }

void Paths::write_pending(char** error_message) {
    std::error_code ec;
    for (std::filesystem::path& path : pending_delete) {
        std::filesystem::remove_all(path, ec);
        CHECKEC(error_message);
    }
    for (std::filesystem::path& path : pending_directory) {
        std::filesystem::create_directory(path, ec);
        CHECKEC(error_message);
    }
    for (PendingRename& pending : pending_rename) {
        std::filesystem::rename(pending.tmp, pending.dest, ec); // may fail on windows
        CHECKEC(error_message);
    }
}