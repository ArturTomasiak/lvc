#include <app.hpp>

bool lvc::diff(int argc, char* argv[]) {
    std::filesystem::path lvc;
    if (!helper::find_repository(lvc))
        return 0;

    LvcError err;
    char** diff;
    LVCEXEC(lvc_diff(lvc.c_str(), &err), diff, err);
    char** diff_copy = diff;

    if (diff && *diff) {
        std::cout << "Files changed since last version: \n";
        while (diff && *diff)
            std::cout << *diff++ << "\n";
        lvc_free_charpp(diff_copy);
    }
    else
        std::cout << "No file changed since last version \n";
    return 1;
}