#include <app.hpp>

bool lvc::move(int argc, char* argv[]) {
    std::filesystem::path lvc;
    if (!helper::find_repository(lvc))
        return 0;

    if (argc < 5) {
        error_message = "Too few arguments; lvc move <workspace_name> <previous_category> <category>";
        return 0;
    }

    LvcError err;
    LVCEXEC(lvc_move_workspace(lvc.c_str(), argv[2], argv[3], argv[4]), err, err);

    std::cout << "Successfully moved " << argv[2] << " from " << argv[3] << " to " << argv[4] << '\n';
    return 1;
}