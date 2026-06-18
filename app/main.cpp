#include <commands.hpp>
#include <file.hpp>
#include <iostream>
#include <state.hpp>
#include <database.hpp>
#include <stdexcept>

State state;
DataBase db;

struct Command {
    const std::string name;
    void (*function)(int argc, char* argv[]);
};

#define COMMANDS_ARRAY { \
    {"help", help}, \
    {"initialize", initialize}, \
    {"tree", tree}, \
    {"history", history}, \
    {"create", create}, \
    {"merge", merge}, \
    {"goto", go_to}, \
    {"status", status}, \
    {"version", version}, \
    {"upload", upload}, \
    {"goback", go_back}, \
    {"remove", remove} \
}

static void (*command(std::string input))(int argc, char* argv[]) {
    Command commands[] = COMMANDS_ARRAY;
    for (const Command& cmd : commands)
        if (input == cmd.name)
            return cmd.function;
    throw std::runtime_error("Invalid argument.");
    return NULL;
}

int main(int argc, char* argv[]) {
    try {
        if (argc < 2) 
            throw std::runtime_error("No argument provided.");
        state.find_lvc();
        if (state.flags & FLAGS_LVC_INITIALIZED)
            db.open();
        command((argv[1]))(argc, argv);
    } catch(const std::exception& e) {
        std::cout <<  e.what() << " For a list of commands lvc help.\n";
        if (state.flags & FLAGS_REMOVE_LVC_ON_FAILURE)
            remove_internal();
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}