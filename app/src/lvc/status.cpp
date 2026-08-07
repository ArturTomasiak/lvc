#include <app.hpp>

bool lvc::status(int argc, char* argv[]) {
    std::filesystem::path lvc;
    if (!helper::find_repository(lvc))
        return 0;

    
    LvcError err;
    char** status = lvc_status(lvc.c_str(), &err);
    if (err) {
        error_message = lvc_error_string(err);
        return 0;
    }
    
    if (status) {
        std::cout << "Files prepared and changed since last version\n";
        while (status)
            std::cout << *status++;
        lvc_free_charpp(status);
    }
    else
        std::cout << "No difference in prepared files since last version";
    return 1;
}