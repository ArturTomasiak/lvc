#include <helper.hpp>

LvcError io::file(std::filesystem::path path, std::ios_base::openmode flags) {
    std::ofstream file(path, flags);
    if (!file)
        return FILE_CREATION_FAILURE;
    file.close();
    return SUCCESS;
}

LvcError io::file(std::filesystem::path path, std::ios_base::openmode flags, const char* content, size_t length, bool compress) {
    std::ofstream file(path, flags);
    if (!file)
        return FILE_CREATION_FAILURE;

    if (compress) {
        size_t deflated_length;
        char* deflated = deflate(content, length, deflated_length);
        if (!deflated)
            return DEFLATION_FAILURE;
        file.write(deflated, deflated_length);
        free(deflated);
    }

    else
        file.write(content, length);
    
    if (!file)
        return FILE_WRITING_FAILURE;
    file.close();
    return SUCCESS;
}

LvcError io::prefix_file_content(std::filesystem::path path, const char* content, size_t length) {
    const std::filesystem::path temp = path.parent_path() / (path.filename().string() + ".tmp");

    std::ifstream input(path, std::ios::binary);
    if (!input)
        return FILE_READING_FAILURE;

    std::ofstream output(temp, std::ios::binary | std::ios::trunc);
    if (!output)
        return FILE_CREATION_FAILURE;

    output.write(content, length);
    output << input.rdbuf();

    if (output.bad()) {
        output.close();
        std::filesystem::remove(temp);
        return FILE_WRITING_FAILURE;
    }

    input.close();
    output.close();

    std::error_code ec;

    std::filesystem::remove(path, ec);
    if (ec) {
        std::filesystem::remove(temp);
        return FILE_WRITING_FAILURE;
    }

    std::filesystem::rename(temp, path, ec);
    if (ec) {
        std::filesystem::remove(temp);
        return FILE_WRITING_FAILURE;
    }

    return SUCCESS;
}

bool io::dir(std::filesystem::path lvc) {
    std::error_code error;
    if (!std::filesystem::create_directory(lvc, error) || error) {
        #ifdef TEST_PRINTS
        if (error)
            std::cout << error.message() << "\n";
        #endif
        return 0;
    }
    return 1;
}

std::string io::content(std::filesystem::path file_path, bool decompress) {
    std::ifstream file(file_path, std::ios::binary);
    std::string out;
    if (!file)
        return out;
    std::ostringstream buffer;
    buffer << file.rdbuf();
    if (!file.eof() && file.fail())
        return out;

    out = buffer.str();
    if (decompress) {
        size_t inflated_len;
        char*    inflated = inflate(out, inflated_len);
        if (inflated)
            out.assign(inflated, inflated_len);
        else
            return {};
    }

    return out;
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

std::string io::content_first_line(std::filesystem::path file_path) {
    std::string result;

    std::ifstream file(file_path, std::ios::binary);
    if (!file.is_open())
        return result;

    std::getline(file, result);
    if (!result.empty() && result.back() == '\r')
        result.pop_back();
    return result;
}

std::vector<std::string> io::content_lines(std::filesystem::path file_path, bool decompress) {
    if (!std::filesystem::exists(file_path))
        return {};
    if (decompress) {
        std::string decompressed = io::content(file_path, decompress);
        if (decompressed.empty())
            return {};

        std::istringstream stream(std::move(decompressed));

        return stream_to_lines(stream);
    }
    else {
        std::ifstream file(file_path, std::ios::binary);
        if (!file.is_open())
            return {};
            
        return stream_to_lines(file);
    }
}