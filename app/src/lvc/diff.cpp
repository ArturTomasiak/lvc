#include <app.hpp>

bool lvc::diff(int argc, char* argv[]) {
    std::filesystem::path lvc;
    if (!helper::find_repository(lvc))
        return 0;

    char** diff = lvc_diff(lvc.c_str());
    if (diff && *diff) {
        std::cout << "Files changed since last version: \n";
        while (diff)
            std::cout << *diff++ << "\n";
    }
    else
        std::cout << "No file changed since last version \n";
    return 1;
}