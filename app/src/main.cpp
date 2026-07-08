#include <string>
#include <iostream>
#include <lvc.hpp>

static std::string error_message;

struct Command {
    const std::string name;
    bool (*function)(int argc, char* argv[]);
};

bool create(int argc, char* argv[]) {
    return 1;
}

#define COMMANDS_ARRAY { \
    {"create", create}, \
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
        std::cout << error_message;
        return EXIT_FAILURE;
    }
}