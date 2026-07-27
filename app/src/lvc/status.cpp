#include <app.hpp>

bool lvc::status(int argc, char* argv[]) {
    std::filesystem::path lvc;
    if (!helper::find_repository(lvc))
        return 0;
    char** status = lvc_status(lvc.c_str());
    if (status)
        std::cout << "Files prepared and changed since last version\n";
    else
        std::cout << "No difference in prepared files since last version";
    while (status)
        std::cout << *status++;
    return 1;
}