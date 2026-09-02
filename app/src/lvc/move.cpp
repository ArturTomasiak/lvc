#include <app.hpp>

bool lvc::move(int argc, char* argv[]) {
    std::filesystem::path lvc;
    if (!helper::find_repository(lvc))
        return 0;

    if (argc < 5) {
        helper::error("Missing arguments, syntax: lvc move <workspace_name> "
                      "<previous_category> <category>");
        return 0;
    }

    lvc_move_workspace(lvc.c_str(), argv[2], argv[3], argv[4], &error_message);

    std::cout << "Successfully moved " << argv[2] << " from " << argv[3] << " to " << argv[4] << '\n';
    return 1;
}