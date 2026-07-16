#include <internal.hpp>

LvcError create::storage(std::filesystem::path directory_root, StorageBehaviour option) {
    directory_root /= "lvc.storage";
    switch(option) {
        case HYBRID: 
            write::file(directory_root, std::ios::binary, DEFAULT_HYBRID_STORAGE);; 
        case CENTRALIZED: 
            write::file(directory_root, std::ios::binary, "0");
        default: ;
    }
    return SUCCESS;
}