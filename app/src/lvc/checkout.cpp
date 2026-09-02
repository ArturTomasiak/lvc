#include <app.hpp>

bool lvc::checkout(int argc, char* argv[]) {
    std::filesystem::path lvc;
    if (!helper::find_repository(lvc)) return 0;

    char** checkout;
    lvc_checkout(lvc.c_str(), &error_message);
    if (error_message) return 0;

    char** checkout_copy = checkout;
    if (checkout && *checkout) {
        std::cout << "Files changed since last version: \n";
        while (checkout && *checkout) std::cout << *checkout++ << "\n";
        lvc_free_charpp(checkout_copy);
    } else
        std::cout << "No file changed since last version \n";
    return 1;
}