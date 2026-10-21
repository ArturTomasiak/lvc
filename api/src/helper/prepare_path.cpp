#include <helper.hpp>
#include <core.hpp>

std::filesystem::path prepare_path(Paths& paths, const std::string& workspace_name) {
    char id[65];
    std::string name = workspace_name + "/" + filename_prepare;
    sha256(name.data(), name.size(), id);
    return paths.object / id;
}