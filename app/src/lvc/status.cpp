#include <app.hpp>

bool lvc::status(int argc, char* argv[]) {
    std::filesystem::path lvc;
    if (!helper::find_repository(lvc))
        return 0;


        
    LvcError err;
    char** status;
    
    if (argc == 3 && !std::strcmp(argv[2], "all"))
        LVCEXEC(lvc_status_all(lvc.c_str()), status, err);
    else
        LVCEXEC(lvc_status(lvc.c_str(), &err), status, err);
    
    char** status_copy = status;

    if (status && *status) {
        std::cout << "Files prepared and changed since last version\n";
        while (status && *status)
            std::cout << *status++ << "\n";
        lvc_free_charpp(status_copy);
    }
    else
        std::cout << "No difference in prepared files since last version\n";
    return 1;
}