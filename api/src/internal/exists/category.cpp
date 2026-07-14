#include <internal.hpp>

bool exists::category(std::filesystem::path lvc, std::string name) {
    lvc /= "workspace";
    lvc /= name;
    return std::filesystem::exists(lvc) && std::filesystem::is_directory(lvc);
}