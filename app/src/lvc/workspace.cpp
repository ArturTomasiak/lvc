#include "app.hpp"

bool lvc::workspace(int argc, char* argv[]) {
    std::filesystem::path lvc;
    if (!helper::find_repository(lvc))
        return 0;
    bool clone_working = 0;

    if (argc < 3) {
        helper::error("No arguments");
        return 0;
    }

    if (argc < 4) {
        helper::error("No workspace name given");
        return 0;
    }

    if (argc == 5) {
        if (!strcmp(argv[4], "copy"))
            clone_working = 1;
        else
            helper::error("Invalid argument" + std::string(argv[4]));
    }

    lvc_workspace(lvc.c_str(), argv[2], argv[3], clone_working, &error_message);
    if (error_message)
        return 0;

    std::cout << "Successfully created workspace " << argv[3] << '\n';
    return 1;
}