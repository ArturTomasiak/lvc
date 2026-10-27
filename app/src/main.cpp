#include <app.hpp>

char* error_message;

struct Command {
    const std::string name;
    bool (*function)(int argc, char* argv[]);
};

constexpr Command commands[] = {
    {"create", lvc::create},     {"version", lvc::version},   {"history", lvc::history},     {"prepare", lvc::prepare},
    {"checkout", lvc::checkout}, {"status", lvc::status},     {"default", lvc::_default},    {"goto", lvc::_goto},
    {"revert", lvc::revert},     {"category", lvc::category}, {"workspace", lvc::workspace}, {"diff", lvc::diff},
    {"current", lvc::current},   {"merge", lvc::merge},       {"list", lvc::list},           {"test", lvc::test}};

static bool (*command(std::string input))(int argc, char* argv[]) {
    for (const Command& cmd : commands)
        if (input == cmd.name)
            return cmd.function;
    helper::error("Invalid argument");
    return 0;
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cout << "No argument provided\n";
        return EXIT_FAILURE;
    }
    bool (*function)(int argc, char* argv[]) = command(argv[1]);
    if (function != nullptr && function(argc, argv))
        return EXIT_SUCCESS;
    else {
        std::cout << error_message << "\nFor more information head to lvc documentation\n";
        free(error_message);
        return EXIT_FAILURE;
    }
}