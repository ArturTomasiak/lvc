#include <core.hpp>

void repository::storage_template(Paths& paths, StorageBehaviour option, char** error_message) {
    switch (option) {
        case HYBRID:
            io::file(
                paths.storage, std::ios::binary, DEFAULT_HYBRID_STORAGE, DEFAULT_HYBRID_STORAGE_LENGTH, 0,
                error_message);
        case CENTRALIZED:
            io::file(paths.storage, std::ios::binary, "0", 1, 0, error_message);
        default:;
    }
}