#include <core.hpp>

void branch::create(Paths& paths, std::string branch_name, std::string copy_branch_name, char** error_message) {
    if (!valid_name(branch_name)) {
        error_message_creator("Dangerous branch name, potential write outside of destination attempt", error_message);
        return;
    }
    const std::filesystem::path branch = paths.branch / branch_name;
    if (std::filesystem::is_regular_file(branch)) {
        error_message_creator("branch already exists", error_message);
        return;
    }
    std::string buffer;
    if (!copy_branch_name.empty())
        buffer = io::content(paths.branch / copy_branch_name, 1, 1, error_message);
    insert_pattern(buffer, TYPE_BRANCH, nullptr);
    const Pending pending = {.type = PendingType::rename, .src = paths.get_tmp_file(), .dest = branch};
    io::file(pending.src, std::ios::binary | std::ios::trunc, buffer.data(), buffer.size(), 1, error_message);
    paths.move_pending(pending);
}