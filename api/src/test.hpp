#include "diff.hpp"
#include "helper.hpp"
#include <iostream>

namespace test {
    bool sha256();
    bool diff();
    bool merge();

    inline void print_lines(const Lines& lines) {
        for (const std::string& line : lines)
            std::cout << line << '\n';
    }
};