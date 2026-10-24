#include <app.hpp>

bool lvc::revert(int argc, char* argv[]) {
    std::filesystem::path lvc;
    if (!helper::find_repository(lvc)) {
        helper::error("Couldn't find LVC repository");
        return 0;
    }

    if (argc < 3) {
        helper::error("No version ID given");
        return 0;
    }

    if (!lvc_version_exists(lvc.c_str(), argv[2])) {
        std::string id = argv[2];
        helper::error("Invalid version id " + id);
        return 0;
    }

    lvc_revert(lvc.c_str(), argv[2], argc - 3, argv + 3, &error_message);
    return 1;
}