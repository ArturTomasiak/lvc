#include <app.hpp>

std::string error_message;

struct Command {
    const std::string name;
    bool (*function)(int argc, char* argv[]);
};

#define COMMANDS_ARRAY { \
    {"create", lvc::create}, \
    {"version", lvc::version}, \
    {"history", lvc::history}, \
    {"prepare", lvc::prepare}, \
    {"diff", lvc::diff}, \
    {"status", lvc::status}, \
    {"default", lvc::_default}, \
    {"goto", lvc::_goto}, \
    {"move", lvc::move}, \
}

static bool (*command(std::string input))(int argc, char* argv[]) {
    Command commands[] = COMMANDS_ARRAY;
    for (const Command& cmd : commands)
        if (input == cmd.name)
            return cmd.function;
    error_message = "Invalid argument.";
    return nullptr;
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
        std::cout << error_message << "\n For more information head to lvc documentation\n";
        return EXIT_FAILURE;
    }
}