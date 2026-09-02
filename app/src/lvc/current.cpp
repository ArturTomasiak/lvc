#include "app.hpp"

bool lvc::current(int argc, char* argv[]) {
    std::filesystem::path lvc;
    if (!helper::find_repository(lvc)) return 0;

    char* name = lvc_current(lvc.c_str(), &error_message);
    if (error_message) return 0;

    std::cout << "Current workspace " << name << '\n';
    free(name);
    return 1;
}