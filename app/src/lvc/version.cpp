#include <app.hpp>

bool lvc::version(int argc, char* argv[]) {
    if (argc < 3) {
        error_message = "No version description given";
        return 0;
    }
    
    std::filesystem::path lvc;
    if (!helper::find_repository(lvc))
        return 0;

    size_t allocate = 0;
    for (size_t i = 2; i < argc; i++)
        allocate += strlen(argv[i]) + 1;
    char* description = (char*)malloc(allocate);
    size_t counter = 0;
    for (size_t i = 2; i < argc; i++) {
        size_t len = strlen(argv[i]);
        memcpy(description + counter, argv[i], len);
        counter += len;
        description[counter++] = ' ';
    }
        
    description[--counter] = '\0';

    LvcError err = lvc_version(lvc.c_str(), description, "TODO");
    free(description);
    if (err) {
        error_message = lvc_error_string(err);
        return 0;
    }
    std::cout << "Successfully created version\n";
    return 1;
}