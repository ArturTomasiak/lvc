#include <app.hpp>

bool helper::find_repository(std::filesystem::path& out) {
    out = std::filesystem::current_path();
    while (true) {
        std::filesystem::path parent = out.parent_path();
        std::filesystem::path lvc    = parent / ".lvc";
        if (out == parent) {
            error_message = "Not in a valid lvc repository";
            return false;
        }
        if (std::filesystem::exists(lvc) && std::filesystem::is_directory(lvc)) {
            out = std::move(lvc);
            return true;
        }
        out = std::move(parent);
    }
}