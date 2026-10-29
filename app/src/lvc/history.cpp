#include <app.hpp>

// options
// depth <num>        - max nested version count (default 1)
// length <num>       - max versions total (default 20)
// workspace <string> - specify workspace  (default current)
// all                - no limit to depth nor length

static void print_history(LvcVersion* version, size_t depth, size_t length) {
    for (size_t i = 0; i < version->len; i++) {
        if (length == 0)
            return;
        LvcVersionEntry* entry = &version->entry[i];
        std::printf("%s %s {%s}\n", entry->id, entry->desc, version->workspace);
        length--;
        if (entry->nested && depth != 0)
            print_history(entry->nested, depth - 1, length);
    }
}

bool lvc::history(int argc, char* argv[]) {
    std::filesystem::path lvc;
    if (!helper::find_repository(lvc)) {
        helper::error("Couldn't find LVC repository");
        return 0;
    }

    std::string workspace;
    bool        all    = 0;
    size_t      depth  = 1;
    size_t      length = 50;

    if (error_message != 0)
        return 0;

    for (int i = 3; i < argc - 1; i++) {
        if (!std::strcmp(argv[i], "depth"))
            depth = std::stoll(argv[++i]);
        else if (!std::strcmp(argv[i], "length"))
            length = std::stoll(argv[++i]);
        else if (!std::strcmp(argv[i], "workspace"))
            workspace = (argv[++i]);
        else if (!std::strcmp(argv[i], "all"))
            all = 1;
        else {
            std::string message = "Unknown option ";
            helper::error(message + argv[i]);
            return 0;
        }
    }

    if (workspace.empty())
        workspace = lvc_current(lvc.c_str(), &error_message);

    if (all) {
        length = SIZE_MAX;
        depth  = SIZE_MAX;
    }

    LvcVersion* version = lvc_history(lvc.c_str(), workspace.c_str(), &error_message);
    print_history(version, depth, length);
    lvc_history_free(version);
    return 1;
}