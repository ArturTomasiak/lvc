#include <app.hpp>

bool lvc::_goto(int argc, char* argv[]) {
    std::filesystem::path lvc;
    if (!helper::find_repository(lvc))
        return 0;

    if (argc < 3) {
        error_message = "No workspace name given";
        return 0;
    }

    LvcError err;
    LVCEXEC(lvc_goto(lvc.c_str(), argv[2]), err, err);
    
    std::cout << "Successfully went to " << argv[2] << '\n';
    return 1;
}