#include "app.hpp"

bool lvc::workspace(int argc, char* argv[]) {
    std::filesystem::path lvc;
    if (!helper::find_repository(lvc))
        return 0;
    bool clean = 0;

    if (argc < 3) {
        helper::error("No workspace name given");
        return 0;
    }

    if (argc == 4) {
        if (!strcmp(argv[3], "clean"))
            clean = 1;
        else
            helper::error("Invalid argument" + std::string(argv[4]));
    }

    lvc_workspace(lvc.c_str(), argv[2], clean, &error_message);
    if (error_message)
        return 0;

    std::cout << "Successfully created workspace " << argv[2] << '\n';
    return 1;
}