#include <helper.hpp>

void io::file(std::filesystem::path path, std::ios_base::openmode flags, char** error_message) {
    std::filesystem::create_directories(path.parent_path());
    std::ofstream file(path, std::ios::out | flags);
    if (!file) {
        error_message_creator("Could not create file " + path.string(), error_message);
        return;
    }
    file.close();
}

void io::file(
    std::filesystem::path path, std::ios_base::openmode flags, const char* content, size_t length, bool compress,
    char** error_message) {
    std::filesystem::create_directories(path.parent_path());
    std::ofstream file(path, std::ios::out | flags);
    if (!file) {
        error_message_creator("Could not create file " + path.string(), error_message);
        return;
    }

    if (compress) {
        size_t deflated_length;
        char*  deflated = deflate(content, length, deflated_length);
        file.write(deflated, deflated_length);
        free(deflated);
    } else
        file.write(content, length);

    if (!file) {
        error_message_creator("Could not write file " + path.string(), error_message);
        return;
    }

    file.close();
}

bool io::dir(std::filesystem::path path, char** error_message) {
    std::error_code error;
    if (!std::filesystem::create_directory(path, error) || error)
        return 0;
    return 1;
}

std::string io::content(std::filesystem::path file_path, bool decompress, bool ignore_pattern, char** error_message) {
    std::ifstream file(file_path, std::ios::binary);
    if (!file)
        return std::string();

    std::ostringstream buffer;
    buffer << file.rdbuf();

    if (!file.eof() && file.fail())
        return std::string();

    if (decompress) {
        size_t inflated_len;
        char*  inflated = inflate(buffer.str(), inflated_len);
        char*  original = inflated;
        if (inflated == 0)
            return std::string();
        if (ignore_pattern) {
            const char* newline = (const char*)memchr(inflated, '\n', inflated_len);
            if (newline) {
                const size_t skipped = newline - inflated + 1;
                inflated += skipped;
                inflated_len -= skipped;
            }
        }
        std::string out = std::string(inflated, inflated_len);
        free(original);
        return out;
    }
    return buffer.str();
}

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

std::vector<std::string>
io::content_lines(std::filesystem::path file_path, bool decompress, bool ignore_pattern, char** error_message) {
    if (!std::filesystem::exists(file_path))
        return {};
    if (decompress) {
        const std::string decompressed = io::content(file_path, decompress, ignore_pattern, error_message);
        if (decompressed.empty())
            return {};

        std::istringstream stream(std::move(decompressed));
        return stream_to_lines(stream);
    } else {
        std::ifstream file(file_path, std::ios::binary);
        if (!file.is_open())
            return {};

        return stream_to_lines(file);
    }
}