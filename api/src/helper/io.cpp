#include <helper.hpp>

void io::file(std::filesystem::path path, std::ios_base::openmode flags, char** error_message) {
    std::filesystem::create_directories(path.parent_path());
    std::ofstream file(path, flags);
    if (!file) {
        error_message_creator_path("Could not create file", path, error_message);
        return;
    }
    file.close();
}

void io::file(std::filesystem::path path, std::ios_base::openmode flags, const char* content, size_t length, bool compress, char** error_message) {
    std::filesystem::create_directories(path.parent_path());
    std::ofstream file(path, flags);
    if (!file) {
        error_message_creator_path("Could not create file", path, error_message);
        return;
    }

    if (compress) {
        size_t deflated_length;
        char*  deflated = deflate(content, length, deflated_length);
        if (!file) {
            error_message_creator_path("Could not deflate file", path, error_message);
            return;
        }
        file.write(deflated, deflated_length);
        free(deflated);
    } else
        file.write(content, length);

    if (!file) {
        error_message_creator_path("Could not write file", path, error_message);
        return;
    }

    file.close();
}

void io::prefix_file_content(std::filesystem::path path, const char* content, size_t length, char** error_message) {
    const std::filesystem::path temp = path.parent_path() / (path.filename().string() + ".tmp");

    std::ifstream input(path, std::ios::binary);
    if (!input) {
        error_message_creator_path("Could not read file", path, error_message);
        return;
    }

    std::ofstream output(temp, std::ios::binary | std::ios::trunc);
    if (!output) {
        error_message_creator_path("Could not create file", temp, error_message);
        return;
    }

    output.write(content, length);
    output << input.rdbuf();

    if (output.bad()) {
        output.close();
        std::filesystem::remove(temp);
        error_message_creator_path("Could not write file", temp, error_message);
        return;
    }

    input.close();
    output.close();

    std::error_code ec;

    std::filesystem::remove(path, ec);
    if (ec) {
        std::filesystem::remove(temp);
        error_message_creator_path("Could not write file", temp, error_message);
        return;
    }

    std::filesystem::rename(temp, path, ec);
    if (ec) {
        std::filesystem::remove(temp);
        error_message_creator_path("Could not write file", path, error_message);
        return;
    }
}

bool io::dir(std::filesystem::path lvc, char** error_message) {
    std::error_code error;
    if (!std::filesystem::create_directory(lvc, error) || error) return 0;
    return 1;
}

std::string io::content(std::filesystem::path file_path, bool decompress, char** error_message) {
    std::ifstream file(file_path, std::ios::binary);
    std::string   out;
    if (!file) return out;
    std::ostringstream buffer;
    buffer << file.rdbuf();
    if (!file.eof() && file.fail()) return out;

    if (decompress) {
        size_t inflated_len;
        char*  inflated = inflate(buffer.str(), inflated_len);
        if (inflated) out.assign(inflated, inflated_len);
    } else
        out = buffer.str();

    return out;
}

static std::vector<std::string> stream_to_lines(std::istream& stream) {
    std::vector<std::string> result;
    result.reserve(PREALLOCATE_SMALL);
    std::string line;
    while (std::getline(stream, line)) {
        if (!line.empty() && line.back() == '\r') line.pop_back();
        result.push_back(line);
    }
    return result;
}

std::string io::content_first_line(std::filesystem::path file_path, char** error_message) {
    std::string result;

    std::ifstream file(file_path, std::ios::binary);
    if (!file.is_open()) return result;

    std::getline(file, result);
    if (!result.empty() && result.back() == '\r') result.pop_back();
    return result;
}

std::vector<std::string> io::content_lines(std::filesystem::path file_path, bool decompress, char** error_message) {
    if (!std::filesystem::exists(file_path)) return {};
    if (decompress) {
        std::string decompressed = io::content(file_path, decompress, error_message);
        if (decompressed.empty()) return {};

        std::istringstream stream(std::move(decompressed));

        return stream_to_lines(stream);
    } else {
        std::ifstream file(file_path, std::ios::binary);
        if (!file.is_open()) return {};

        return stream_to_lines(file);
    }
}