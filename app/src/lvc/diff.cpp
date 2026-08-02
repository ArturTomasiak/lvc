#include <app.hpp>

bool lvc::diff(int argc, char* argv[]) {
    std::filesystem::path lvc;
    if (!helper::find_repository(lvc))
        return 0;

    LvcError err;
    char** diff = lvc_diff(lvc.c_str(), &err);
    if (err) {
        error_message = lvc_error_string(err);
        return 0;
    }
    if (diff && *diff) {
        std::cout << "Files changed since last version: \n";
        while (diff)
            std::cout << *diff++ << "\n";
    }
    else
        std::cout << "No file changed since last version \n";
    return 1;
}