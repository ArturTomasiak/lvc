#include <internal.hpp>

LvcError create::storage(std::filesystem::path directory_root, StorageBehaviour option) {
    directory_root /= "lvc.storage";
    switch(option) {
        case HYBRID: 
            write::file(directory_root, std::ios::binary, DEFAULT_HYBRID_STORAGE, DEFAULT_HYBRID_STORAGE_LENGTH);
        case CENTRALIZED: 
            write::file(directory_root, std::ios::binary, "0", 1);
        default: ;
    }
    return SUCCESS;
}