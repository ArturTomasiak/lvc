#include "core.hpp"
#include <array>

// this works only as long as the order in operation_arr is the same as the
// order in operation::type

static constexpr const std::string* operation_vector[] = {&filename_sync, &filename_unite, &filename_insert, nullptr};

operation::type
operation::ongoing(Paths& paths, const std::string& workspace_name, std::string& operation_str, char** error_message) {
    std::filesystem::path local = paths.local / workspace_name / foldername_operation;
    if (!std::filesystem::is_directory(local))
        return operation::type::none;
    uint8_t i = 0;
    for (const std::string* address = operation_vector[i++]; address != nullptr;) {
        if (std::filesystem::is_regular_file(local / *address)) {
            operation_str = *address;
            return (operation::type)(i + 1);
        }
    }
    error_message_creator("Corrupted local directory: " + local.string() + " will be deleted", error_message);
    std::filesystem::remove_all(local);
    return operation::type::none;
}