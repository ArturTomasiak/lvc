#include <test.hpp>

struct ExpectedDiff {
    Lines             base, side;
    std::vector<Edit> edits;
};

static bool compare(const ExpectedDiff& expected) {
    std::vector<Edit> actual = diff(expected.base, expected.side);
    if (actual.size() != expected.edits.size())
        return false;
    for (size_t i = 0; i < actual.size(); i++) {
        if (actual[i].base_begin != expected.edits[i].base_begin || actual[i].base_end != expected.edits[i].base_end ||
            actual[i].side_begin != expected.edits[i].side_begin || actual[i].side_end != expected.edits[i].side_end)
            return false;
    }
    return true;
}

bool test::diff() {
    bool                            out   = true;
    const std::vector<ExpectedDiff> tests = {
        {{}, {}, {}},
        {{"a", "b", "c"}, {"a", "b", "c"}, {}},
        {{}, {"a", "b"}, {{0, 0, 0, 2}}},
        {{"a", "b"}, {}, {{0, 2, 0, 0}}},
        {{"a", "b"}, {"x", "a", "b"}, {{0, 0, 0, 1}}},
        {{"a", "b"}, {"a", "b", "x"}, {{2, 2, 2, 3}}},
        {{"a", "b"}, {"a", "x", "y", "b"}, {{1, 1, 1, 3}}},
        {{"a", "b", "c"}, {"b", "c"}, {{0, 1, 0, 0}}},
        {{"a", "b", "c"}, {"a", "b"}, {{2, 3, 2, 2}}},
        {{"a", "b", "c", "d"}, {"a", "d"}, {{1, 3, 1, 1}}},
        {{"a", "b", "c"}, {"a", "x", "c"}, {{1, 2, 1, 2}}},
        {{"a", "b"}, {"x", "y"}, {{0, 2, 0, 2}}},
        {{"a", "b", "c"}, {"a", "x", "y", "c"}, {{1, 2, 1, 3}}},
        {{"a", "b", "c", "d"}, {"a", "x", "d"}, {{1, 3, 1, 2}}},
        {{"a", "b", "c", "d", "e"}, {"x", "b", "y", "d", "z"}, {{0, 1, 0, 1}, {2, 3, 2, 3}, {4, 5, 4, 5}}},
        {{"a", "b", "c", "d", "e"}, {"a", "x", "y", "b", "c", "e"}, {{1, 1, 1, 3}, {3, 4, 5, 5}}},
        {{"b"}, {"a", "b", "c"}, {{0, 0, 0, 1}, {1, 1, 2, 3}}},
        {{"a", "b", "c"}, {"b"}, {{0, 1, 0, 0}, {2, 3, 1, 1}}},
        {{"a", "a", "b", "a"}, {"a", "b", "a"}, {{1, 2, 1, 1}}},
        {{"abc", " abc", ""}, {"ABC", "abc ", ""}, {{0, 2, 0, 2}}},
        {{std::string("a\0b", 3)}, {std::string("a\0c", 3)}, {{0, 1, 0, 1}}},
    };
    for (const ExpectedDiff& expected : tests) {
        if (!compare(expected)) {
            std::cout << "DIFF TEST FAILURE:\nBASE:\n";
            print_lines(expected.base);
            std::cout << "SIDE:\n";
            print_lines(expected.side);
            out = false;
        }
    }
    return out;
}