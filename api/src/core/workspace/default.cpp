#include "core.hpp"

void branch::_default(Paths& paths, const char* branch_name, char** error_message) {
    if (!std::filesystem::is_regular_file(paths.branch / branch_name)) {
        error_message_creator("Branch " + std::string(branch_name) + " doesn't exist", error_message);
        return;
    }
    const Pending pending = {.type = PendingType::rename, .src = paths.get_tmp_file(), .dest = paths.lvc / filename_default};
    io::file(pending.src, std::ios::binary | std::ios::trunc, branch_name, charplen(branch_name), 0, error_message);
    paths.move_pending(pending);
}