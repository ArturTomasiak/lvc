#include <app.hpp>

bool lvc::version(int argc, char* argv[]) {
    std::filesystem::path lvc;
    if (!helper::find_repository(lvc))
        return 0;
    
    if (argc < 3) {
        error_message = "No version description given";
        return 0;
    }
    // TODO
    return 1;
}