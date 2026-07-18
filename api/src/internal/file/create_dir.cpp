#include <internal.hpp>

bool file::create_dir(std::filesystem::path lvc) {
    std::error_code error;
    if (!std::filesystem::create_directory(lvc, error) || error) {
        #ifdef TEST_PRINTS
        if (error)
            std::cout << error.message() << "\n";
        #endif
        return 0;
    }
    return 1;
}