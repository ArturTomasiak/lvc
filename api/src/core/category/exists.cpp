#include <core.hpp>

bool category::exists(Paths& paths, std::string name) { return std::filesystem::is_directory(paths.workspace / name); }