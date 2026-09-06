#include "core.hpp"

// this works only as long as the order in operation_arr is the same as the
// order in operation::type

static char* operation_arr[] = {
    (char*)&operation::filename_sync, (char*)&operation::filename_unite, (char*)&operation::filename_insert, nullptr};

operation::type operation::ongoing(Paths& paths, std::string& workspace_name, char** error_message) {
    std::filesystem::path local = paths.local / workspace_name;
    if (!std::filesystem::is_directory(local))
        return operation::type::none;
    for (int i = 0; operation_arr[i] != nullptr; i++)
        if (std::filesystem::is_regular_file(local / operation_arr[i]))
            return (operation::type)(i + 1);
    error_message_creator("Corrupted local directory: " + local.string() + " will be deleted", error_message);
    std::filesystem::remove_all(local);
    return operation::type::none;
}