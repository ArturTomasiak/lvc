#include <app.hpp>

bool lvc::_default(int argc, char* argv[]) {
    std::filesystem::path lvc;
    if (!helper::find_repository(lvc))
        return 0;
    
    if (argc < 3) {
        error_message = "No workspace name given";
        return 0;
    }

    LvcError err;
    LVCEXEC(lvc_default(lvc.c_str(), argv[2]), err, err);
    
    std::cout << "Successfully made " << argv[2] << " default\n";
    return 1;
}