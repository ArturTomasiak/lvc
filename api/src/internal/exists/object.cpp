#include <internal.hpp>

bool object (std::filesystem::path object_dir, char id[65]) {
    return std::filesystem::is_regular_file(object_dir / id);
}