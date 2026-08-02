#include <app.hpp>

bool lvc::version(int argc, char* argv[]) {
    std::filesystem::path lvc;
    if (!helper::find_repository(lvc))
        return 0;
    
    if (argc < 3) {
        error_message = "No version description given";
        return 0;
    }
    
    LvcError err = lvc_version(lvc.c_str(), argv[2], "TODO");
    if (err) {
        error_message = lvc_error_string(err);
        return 0;
    }
    return 1;
}