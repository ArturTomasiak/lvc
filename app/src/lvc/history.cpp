#include <app.hpp>

static void print_history(LvcVersion* version, std::string prefix = "") {
    while (version) {
        std::cout << version->id << " " << version->description << "\n";
        print_history(version->nested_versions, prefix += "   ");
        version = version->previous;
    }
}

bool lvc::history(int argc, char* argv[]) {
    if (argc < 3) {
        error_message = "No workspace name given";
        return 0;
    }

    std::filesystem::path lvc;
    if (!helper::find_repository(lvc))
        return 0;
    
    LvcVersion* version = lvc_history(lvc.c_str(), argv[2]);
    print_history(version);
    lvc_history_free(version);
    return 1;
}