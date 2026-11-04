#include <paths.hpp>

std::filesystem::path Paths::get_tmp_file() { return tmp / std::to_string(tmp_counter++); }