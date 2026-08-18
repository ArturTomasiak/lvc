#include <app.hpp>

bool lvc::version(int argc, char* argv[]) {
    if (argc < 3) {
        error_message = "No version description given";
        return 0;
    }
    
    std::filesystem::path lvc;
    if (!helper::find_repository(lvc))
        return 0;
    
    LvcError err;
    LVCEXEC(lvc_version(lvc.c_str(), argv[2], "TODO"), err, err);
    std::cout << "Successfully created version\n";
    return 1;
}