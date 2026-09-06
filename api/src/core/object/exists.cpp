#include <core.hpp>

bool object::exists(Paths& paths, char id[65]) { return std::filesystem::is_regular_file(paths.object / id); }