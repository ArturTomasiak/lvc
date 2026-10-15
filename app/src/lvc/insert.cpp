#include <app.hpp>

bool lvc::insert(int argc, char* argv[]) {
    std::filesystem::path lvc;
    if (!helper::find_repository(lvc))
        return 0;

    if (argc < 3) {
        helper::error("No src workspace given.");
        return 0;
    }

    lvc_insert(lvc.c_str(), argv[2], "placeholder", &error_message);
    return 1;
}