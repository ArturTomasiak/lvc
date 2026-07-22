#include <core.hpp>

static std::vector<std::string> stream_to_lines(std::istream& stream) {
    std::vector<std::string> result;
    result.reserve(PREALLOCATE_SMALL);
    std::string line;
    while (std::getline(stream, line)) {
        if (!line.empty() && line.back() == '\r')
            line.pop_back();
        result.push_back(line);
    }
    return result;
}

std::vector<std::string> file::content_lines(std::filesystem::path file_path, bool deflated) {
    std::vector<std::string> empty;
    if (deflated) {
        std::string decompressed = file::content(file_path, deflated);
        if (decompressed.empty())
            return empty;

        std::istringstream stream(std::move(decompressed));

        return stream_to_lines(stream);
    }
    else {
        std::ifstream file(file_path, std::ios::binary);
        if (!file.is_open())
            return empty;
            
        return stream_to_lines(file);
    }
}