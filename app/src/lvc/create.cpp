#include <app.hpp>

static std::string ask_line(const std::string& question, bool allow_empty) {
    std::string answer;
    while (true) {
        std::cout << question;
        std::getline(std::cin, answer);
        if (allow_empty || !answer.empty())
            return answer;
        std::cout << "Value cannot be empty.\n";
    }
}

static bool ask_local() {
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

static StorageBehaviour ask_storage() {
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

static void ask_clone(std::string& clone, bool& clone_versioning) {
    clone = ask_line("Clone repository path/link (leave empty to start from scratch) ", 1);
    if (clone.empty())
        return;

    std::string answer;
    while (true) {
        answer = ask_line("Clone the .lvc folder as well? Y/N ", 0);
        answer = std::tolower(answer[0]);
        if (answer == "y") {
            clone_versioning = 1;
            return;
        }
        if (answer == "n") {
            clone_versioning = 0;
            return;
        }
        std::cout << "Invalid response, type Y or N.\n";
    }
}

bool lvc::create(int argc, char* argv[]) {
    LvcCreateInput input{};
    std::string location = std::filesystem::current_path();
    std::string server_link;
    std::string repository_name;
    std::string category_name;
    std::string workspace_name;
    std::string clone;
    
    input.local = ask_local();
    if (!input.local) {
        server_link = ask_line("Server link: ", 0);
        input.storage_behaviour = ask_storage();
    }
    ask_clone(clone, input.clone_versioning);
    if (input.clone_versioning) 
        goto skip_verioning_questions;

    repository_name = ask_line("Repository name ", 0);
    category_name = ask_line("Create default workspace’s category name ", 0);
    workspace_name = ask_line("Create default workspace name ", 0);
    
skip_verioning_questions:
    input.location         = location.c_str();
    input.repository_name  = repository_name.c_str();
    input.server_link      = server_link.c_str();
    input.category_name    = category_name.c_str();
    input.workspace_name   = workspace_name.c_str();
    input.clone_repository = clone.empty() ? 0 : clone.c_str();

    LvcError err;
    LVCEXEC(lvc_create(input), err, err);
    std::cout << "Successfully created repository\n";
    return 1;
}