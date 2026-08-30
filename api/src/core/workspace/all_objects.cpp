#include <core.hpp>

std::vector<object::info> workspace::all_objects(std::filesystem::path working_directory, char** error_message) {
    std::vector<object::info> out;
    out.reserve(PREALLOCATE);

    try {
        std::filesystem::recursive_directory_iterator iterator(working_directory);
        std::filesystem::recursive_directory_iterator end;

        for(const std::filesystem::directory_entry& entry = *iterator; iterator != end; iterator++) {
            if(entry.is_directory() && entry.path().filename() == ".lvc") {
                iterator.disable_recursion_pending();
                continue;
            }

            object::info object;
            object.path = entry.path().lexically_relative(working_directory);
            object.type = std::filesystem::is_regular_file(entry.path()) ? object::type::blob : object::type::tree;

            std::string buffer = io::content(entry.path(), 0, error_message);
            insert_pattern(buffer, TYPE_BLOB);

            char id[65];
            sha256(buffer.data(), buffer.size(), id);

            object.id = id;
            out.push_back(std::move(object));
        }
    } catch(const std::filesystem::filesystem_error& error) {
        error_message_creator("Directory iteration failure", error_message);
    }

    return out;
}