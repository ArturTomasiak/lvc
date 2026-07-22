#include <app.hpp>

bool lvc::prepare(int argc, char* argv[]) {
    std::filesystem::path lvc;
    if (!helper::find_repository(lvc))
        return 0;

    LvcError err = lvc_prepare(lvc.c_str(), argc, argv);
    if (err) {
        error_message = lvc_error_string(err);
        return 0;
    }
    return 1;
}