#include <commands.hpp>
#include <database.hpp>
#include <state.hpp>
#include <file.hpp>
#include <iostream>

void help(int argc, char* argv[]) {
    ;
}

void initialize(int argc, char* argv[]) {
    if (state.flags & FLAGS_LVC_INITIALIZED)
        throw std::runtime_error("LVC was already initialized.");
    state.flags |= FLAGS_REMOVE_LVC_ON_FAILURE;
    create_folder(".lvc/objects");
    state.lvc_directory = ".lvc";
    state.config_path = "lvc.config";
    create_file("lvc.config", default_settings);
    db.create_new();
    state.flags &= ~FLAGS_REMOVE_LVC_ON_FAILURE;
}

void tree(int argc, char* argv[]) {
    ;
}

void history(int argc, char* argv[]) {
    ;
}

void create(int argc, char* argv[]) {
    ;
}

void merge(int argc, char* argv[]) {
    ;
}

void go_to(int argc, char* argv[]) {
    ;
}

void status(int argc, char* argv[]) {
    ;
}

void version(int argc, char* argv[]) {
    ;
}

void upload(int argc, char* argv[]) {
    ;
}

void go_back(int argc, char* argv[]) {
    ;
}

void remove_internal() {
    delete_folder(state.lvc_directory.c_str());
    delete_file(state.config_path.c_str());
}

void remove(int argc, char* argv[]) {
    if (!(state.flags & FLAGS_LVC_INITIALIZED))
        throw std::runtime_error("Could not find lvc.");
    remove_internal();
    std::cout << "Removed lvc from project." << "\n";
}