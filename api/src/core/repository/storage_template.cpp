#include <core.hpp>

void repository::storage_template(Paths& paths, StorageBehaviour option, char** error_message) {
    PendingRename pending = {.tmp = paths.tmp / filename_storage, .dest = paths.storage};
    switch (option) {
        case HYBRID:
            io::file(
                pending.tmp, std::ios::binary | std::ios::trunc, DEFAULT_HYBRID_STORAGE, DEFAULT_HYBRID_STORAGE_LENGTH,
                0, error_message);
            break;
        case CENTRALIZED:
            io::file(pending.tmp, std::ios::binary | std::ios::trunc, "0", 1, 0, error_message);
            break;
        default:
            return;
    }
    paths.pending_rename.emplace_back(std::move(pending));
}