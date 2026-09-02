#include "core.hpp"

// this works only as long as the order in operation_arr is the same as the
// order in operation::type

static char* operation_arr[] = {(char*)&operation::filename_sync, (char*)&operation::filename_unite, (char*)&operation::filename_insert, nullptr};

operation::type operation::ongoing(const std::filesystem::path& operation_dir, char** error_message) {
    if (!std::filesystem::is_directory(operation_dir)) return operation::type::none;
    for (int i = 0; operation_arr[i] != nullptr; i++)
        if (std::filesystem::is_regular_file(operation_dir / operation_arr[i])) return (operation::type)(i + 1);
    error_message_creator("Corrupted ./lvc/operation directory; ./lvc/operation will be deleted", error_message);
    std::filesystem::remove_all(operation_dir);
    return operation::type::none;
}