#include <app.hpp>

bool helper::find_repository(std::filesystem::path& out) {
    out = std::filesystem::current_path();
    std::filesystem::path previous;
    while (true) {
        std::filesystem::path lvc = out / ".lvc";
        if (out == previous) {
            helper::error("Not in a valid lvc repository");
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

void helper::error(std::string message) {
    error_message = (char*)malloc(message.size() + 1);
    memcpy(error_message, message.data(), message.size());
    error_message[message.size()] = '\0';
}