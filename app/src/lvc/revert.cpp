#include <app.hpp>

bool lvc::revert(int argc, char* argv[]) {
    std::filesystem::path lvc;
    if (!helper::find_repository(lvc))
        return 0;

    if (!std::filesystem::is_regular_file(lvc / NAME_OBJECT / argv[2])) {
        error_message = "Invalid version id";
        return 0;
    }

    LvcError err;
    LVCEXEC(lvc_revert(lvc.c_str(), argv[2], argc - 3, argv + 3), err, err);
    return 1;
}