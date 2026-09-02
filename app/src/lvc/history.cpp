#include <app.hpp>

// options
// depth <num>  - max nested version count (default 1)
// length <num> - max versions total (default 50)
// all          - no limit to depth nor length

static void print_history(LvcVersion* version, std::string prefix = "") {
    while (version) {
        std::cout << prefix << version->id << " " << version->description << "\n";
        if (version->nested_versions) print_history(version->nested_versions, prefix + "   ");
        version = version->previous;
    }
}

bool lvc::history(int argc, char* argv[]) {
    if (argc < 3) {
        helper::error("No workspace name given");
        return 0;
    }

    std::filesystem::path lvc;
    if (!helper::find_repository(lvc)) return 0;

    bool   all    = 0;
    size_t depth  = 1;
    size_t length = 50;

    for (int i = 3; i < argc - 1; i++) {
        if (!std::strcmp(argv[i], "depth"))
            depth = std::stoll(argv[++i]);
        else if (!std::strcmp(argv[i], "length"))
            length = std::stoll(argv[++i]);
        else if (!std::strcmp(argv[i], "all"))
            all = 1;
        else {
            std::string message = "Unknown option ";
            helper::error(message + argv[i]);
            return 0;
        }
    }

    LvcVersion* version = lvc_history(lvc.c_str(), argv[2], depth, length, all, &error_message);
    print_history(version);
    lvc_history_free(version);
    return 1;
}