#include <app.hpp>

bool lvc::revert(int argc, char* argv[]) {
    std::filesystem::path lvc;
    if (!helper::find_repository(lvc)) return 0;

    if (!std::filesystem::is_regular_file(lvc / NAME_OBJECT / argv[2])) {
        std::string id = argv[2];
        helper::error("Invalid version id " + id);
        return 0;
    }

    lvc_revert(lvc.c_str(), argv[2], argc - 3, argv + 3, &error_message);
    return 1;
}