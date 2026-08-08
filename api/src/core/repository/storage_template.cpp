#include <core.hpp>

LvcError repository::storage_template(std::filesystem::path directory_root, StorageBehaviour option) {
    directory_root /= NAME_STORAGE;
    switch(option) {
        case HYBRID: 
            RETURN_ERR(io::file(directory_root, std::ios::binary, DEFAULT_HYBRID_STORAGE, DEFAULT_HYBRID_STORAGE_LENGTH, 0));
        case CENTRALIZED: 
            RETURN_ERR(io::file(directory_root, std::ios::binary, "0", 1, 0));
        default: ;
    }
    return SUCCESS;
}