#include <app.hpp>

bool lvc::_goto(int argc, char* argv[]) {
    std::filesystem::path lvc;
    if (!helper::find_repository(lvc))
        return 0;

    if (argc < 3) {
        helper::error("No workspace name given");
        return 0;
    }

    lvc_goto(lvc.c_str(), argv[2], &error_message);
    if (error_message) return 0;
    
    std::cout << "Successfully went to " << argv[2] << '\n';
    return 1;
}