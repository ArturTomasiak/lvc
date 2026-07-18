#include <internal.hpp>

LvcError repository::storage_template(std::filesystem::path directory_root, StorageBehaviour option) {
    directory_root /= NAME_STORAGE;
    switch(option) {
        case HYBRID: 
            file::create(directory_root, std::ios::binary, DEFAULT_HYBRID_STORAGE, DEFAULT_HYBRID_STORAGE_LENGTH, 0);
        case CENTRALIZED: 
            file::create(directory_root, std::ios::binary, "0", 1, 0);
        default: ;
    }
    return SUCCESS;
}