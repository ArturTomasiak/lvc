#include <app.hpp>

bool helper::find_repository(std::filesystem::path& out) {
    out = std::filesystem::current_path();
    std::filesystem::path previous;
    while (true) {
        std::filesystem::path lvc = out / ".lvc";
        if (out == previous) {
            error_message = "Not in a valid lvc repository";
            return false;
        }
        if (std::filesystem::is_directory(lvc)) {
            out = std::move(lvc);
            return true;
        }
        previous = out;
        out = out.parent_path();
    }
}