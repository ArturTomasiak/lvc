#include <string>
#include <iostream>
#include <lvc.hpp>
#include <cctype>
#include <filesystem>

static std::string error_message;

struct Command {
    const std::string name;
    bool (*function)(int argc, char* argv[]);
};

std::string ask_line(const std::string& question, bool allow_empty) {
    std::string answer;
    while (true) {
        std::cout << question;
        std::getline(std::cin, answer);
        if (allow_empty || !answer.empty())
            return answer;
        std::cout << "Value cannot be empty.\n";
    }
}

bool ask_local() {
    std::string answer;
    while (true) {
        answer = ask_line("Local or server repository? L/R ", 0);
        answer = std::tolower(answer[0]);
        if (answer == "l")
            return 1;
        if (answer == "r")
            return 0;
        std::cout << "Invalid response, type L or R.\n";
    }
}

StorageBehaviour ask_storage() {
    std::string answer;
    int behaviour = 0;
    while (true) {
        answer = ask_line(
            "Choose storage behaviour 0/1/2\n0 no custom storage behaviour (distributed)\n"
            "1 lvc.server file with defaults for extensions and sizes to be stored on the server side (hybrid)\n"
            "2 all files stored on the server (centralized)\n", 
            0
        );
        try {behaviour = std::stoi(answer);} catch(const std::exception&) {}
        if (behaviour == 1 || behaviour == 2 || behaviour == 3)
            return (StorageBehaviour)behaviour;
        std::cout << "Invalid response, type 1, 2 or 3.\n";
    }
}

bool create(int argc, char* argv[]) {
    LvcCreateInput input{};
    std::string location = std::filesystem::current_path();
    std::string server_link;
    std::string repository_name;
    std::string category_name;
    std::string workspace_name;
    std::string clone;
    
    input.local = ask_local();
    if (!input.local) {
        server_link = ask_line("Server link: ", 0).c_str();
        input.storage_behaviour = ask_storage();
    }
    repository_name = ask_line("Repository name ", 0);
    category_name = ask_line("Create default workspace’s category name ", 0);
    workspace_name = ask_line("Create default workspace name ", 0);
    clone = ask_line("Clone repository link (leave empty to start from scratch) ", 1);

    input.location         = location.c_str();
    input.repository_name  = repository_name.c_str();
    input.server_link      = server_link.c_str();
    input.category_name    = category_name.c_str();
    input.workspace_name   = workspace_name.c_str();
    input.clone_repository = clone.empty() ? 0 : clone.c_str();

    LvcError err = lvc_create(input);
    if (err) {
        error_message = lvc_error_string(err);
        return 0;
    }
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
        std::cout << error_message << "\n";
        return EXIT_FAILURE;
    }
}