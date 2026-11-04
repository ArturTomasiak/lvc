#include <app.hpp>

bool lvc::merge(int argc, char* argv[]) {
    std::filesystem::path lvc;
    if (!helper::find_repository(lvc, std::filesystem::current_path())) {
        helper::error("Couldn't find LVC repository");
        return 0;
    }

    if (argc < 3) {
        helper::error("No src workspace given.");
        return 0;
    }

    if (argc > 4) {
        helper::error("Too many arguments.");
        return 0;
    }

    std::filesystem::path src_lvc;
    if (argc == 4 && !helper::find_repository(src_lvc, argv[3])) {
        helper::error("Couldn't find src LVC repository");
        return 0;
    }

    lvc_merge(lvc.c_str(), argv[2], src_lvc.empty() ? 0 : src_lvc.c_str(), &error_message);
    return error_message == 0;
}