#include <core.hpp>

void repository::storage_template(Paths& paths, StorageBehaviour option, char** error_message) {
    const std::filesystem::path tmp = paths.tmp_file();
    switch (option) {
        case HYBRID:
            io::file(
                tmp, std::ios::binary | std::ios::trunc, DEFAULT_HYBRID_STORAGE, DEFAULT_HYBRID_STORAGE_LENGTH, 0,
                error_message);
            break;
        case CENTRALIZED:
            io::file(tmp, std::ios::binary | std::ios::trunc, "0", 1, 0, error_message);
            break;
        default:
            return;
    }
    paths.add_pending({.type = PendingType::rename, .src = tmp, .dest = paths.storage});
}