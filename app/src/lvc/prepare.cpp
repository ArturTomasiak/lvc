#include <app.hpp>

bool lvc::prepare(int argc, char* argv[]) {
    std::filesystem::path lvc;
    if (!helper::find_repository(lvc))
        return 0;

    LvcError err;
    char** prepared_files = lvc_prepare(lvc.c_str(), argc, argv, &err);
    if (err) {
        error_message = lvc_error_string(err);
        return 0;
    }

    if (prepared_files)
        std::cout << "Succesfully prepared " << prepared_files << " files\nFor more information check paths at .lvc/status";
    else
        std::cout << "No files were prepared";
    return 1;
}