#include <app.hpp>

static bool call_reset(const std::filesystem::path& lvc) {
    LvcError err = lvc_prepare_reset(lvc.c_str());
    if (err) {
        error_message = lvc_error_string(err);
        return 0;
    }
    std::cout << "Succesfully removed all files from prepare\n";
    return 1;
}

bool lvc::prepare(int argc, char* argv[]) {
    std::filesystem::path lvc;
    if (!helper::find_repository(lvc))
        return 0;

    if (argc == 3 && !std::strcmp(argv[2], "reset"))
        return call_reset(lvc);

    LvcError err;
    char** prepared_files = lvc_prepare(lvc.c_str(), argc, argv, &err);
    if (err) {
        error_message = lvc_error_string(err);
        return 0;
    }

    char** prepared_files_copy = prepared_files;
    
    if (prepared_files) {
        std::cout << "Succesfully prepared following files:\n";
        while (prepared_files)
            std::cout << *prepared_files++ << "\n";
        lvc_free_charpp(prepared_files_copy);
    }
    else
        std::cout << "No files were prepared";
    return 1;
}