#include <core.hpp>

void repository::storage_template(std::filesystem::path directory_root, StorageBehaviour option, char** error_message) {
    directory_root /= NAME_STORAGE;
    switch(option) {
        case HYBRID:
            io::file(directory_root, std::ios::binary, DEFAULT_HYBRID_STORAGE, DEFAULT_HYBRID_STORAGE_LENGTH, 0, error_message);
        case CENTRALIZED: 
            io::file(directory_root, std::ios::binary, "0", 1, 0, error_message);
        default: ;
    }
}