#include <app.hpp>

bool lvc::_default(int argc, char* argv[]) {
    std::filesystem::path lvc;
    if (!helper::find_repository(lvc))
        return 0;
    
    if (argc < 3) {
        error_message = "No workspace name given";
        return 0;
    }

    LvcError err = lvc_default(lvc.c_str(), argv[2]);
    if (err) {
        error_message = lvc_error_string(err);
        return 0;
    }
    
    std::cout << "Successfully made " << argv[2] << " default\n";
    return 1;
}