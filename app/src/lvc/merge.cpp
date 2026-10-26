#include <app.hpp>

bool lvc::merge(int argc, char* argv[]) {
    std::filesystem::path lvc;
    if (!helper::find_repository(lvc)) {
        helper::error("Couldn't find LVC repository");
        return 0;
    }

    if (argc < 3) {
        helper::error("No src workspace given.");
        return 0;
    }

    lvc_merge(lvc.c_str(), argv[2], "placeholder", &error_message);
    return 1;
}