#include <app.hpp>

bool lvc::version(int argc, char* argv[]) {
    std::filesystem::path lvc;
    if (!helper::find_repository(lvc, std::filesystem::current_path())) {
        helper::error("Couldn't find LVC repository");
        return 0;
    }

    lvc_version(lvc.c_str(), argc, argv, "TODO", &error_message);

    return error_message == 0;
}