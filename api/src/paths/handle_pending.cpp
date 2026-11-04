#include <paths.hpp>

void Paths::handle_pending(const Pending& pending) {
    std::error_code ec;
    switch (pending.type) {
        case PendingType::remove_all:
            std::filesystem::remove_all(pending.dest, ec);
            break;
        case PendingType::remove:
            std::filesystem::remove(pending.dest, ec);
            break;
        case PendingType::create_directory:
            std::filesystem::create_directory(pending.dest, ec);
            break;
        case PendingType::create_directories:
            std::filesystem::create_directories(pending.dest, ec);
        case PendingType::rename:
            std::filesystem::rename(pending.src, pending.dest, ec);
            break;
    }
    if (ec)
        error_message_creator(ec.message(), error_message);
}