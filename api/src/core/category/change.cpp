#include <core.hpp>

void category::change(Paths& paths, std::string branch_name, const char* name, char** error_message) {
    const std::string branch = paths.branch / branch_name;
    if (!std::filesystem::is_regular_file(branch)) [[unlikely]] {
        error_message_creator("branch " + branch_name + " does not exists.", error_message);
        return;
    }
    std::string buffer = io::content(branch, 1, 1, error_message);
    insert_pattern(buffer, TYPE_BRANCH, name);
    const Pending pending = {.type = PendingType::rename, .src = paths.get_tmp_file(), .dest = branch};
    io::file(pending.src, std::ios::binary | std::ios::trunc, buffer.data(), buffer.size(), 1, error_message);
    paths.move_pending(pending);
}