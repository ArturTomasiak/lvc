#include <app.hpp>

bool lvc::diff(int argc, char* argv[]) {
    std::filesystem::path lvc;
    if (!helper::find_repository(lvc)) {
        helper::error("Couldn't find LVC repository");
        return 0;
    }

    lvc_diff(lvc.c_str(), argc, argv, &error_message);
    if (error_message)
        return 0;
    return 1;
}