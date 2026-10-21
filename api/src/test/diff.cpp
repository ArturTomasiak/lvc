#include <test.hpp>

struct Expected {
    const char*       name;
    Lines             base, side;
    std::vector<Edit> edits;
};

bool compare(const std::vector<Edit>& actual, const std::vector<Edit>& expected) {
    if (actual.size() != expected.size())
        return false;
    for (size_t i = 0; i < actual.size(); i++) {
        if (actual[i].base_begin != expected[i].base_begin || actual[i].base_end != expected[i].base_end ||
            actual[i].side_begin != expected[i].side_begin || actual[i].side_end != expected[i].side_end)
            return false;
    }
    return true;
}

bool test::diff() {
    bool                        out   = true;
    const std::vector<Expected> tests = {
        {"empty", {}, {}, {}},
        {"unchanged", {"a", "b", "c"}, {"a", "b", "c"}, {}},
        {"insert into empty", {}, {"a", "b"}, {{0, 0, 0, 2}}},
        {"delete everything", {"a", "b"}, {}, {{0, 2, 0, 0}}},
        {"prepend", {"a", "b"}, {"x", "a", "b"}, {{0, 0, 0, 1}}},
        {"append", {"a", "b"}, {"a", "b", "x"}, {{2, 2, 2, 3}}},
        {"insert middle", {"a", "b"}, {"a", "x", "y", "b"}, {{1, 1, 1, 3}}},
        {"delete first", {"a", "b", "c"}, {"b", "c"}, {{0, 1, 0, 0}}},
        {"delete last", {"a", "b", "c"}, {"a", "b"}, {{2, 3, 2, 2}}},
        {"delete middle", {"a", "b", "c", "d"}, {"a", "d"}, {{1, 3, 1, 1}}},
        {"replace one", {"a", "b", "c"}, {"a", "x", "c"}, {{1, 2, 1, 2}}},
        {"replace everything", {"a", "b"}, {"x", "y"}, {{0, 2, 0, 2}}},
        {"grow replacement", {"a", "b", "c"}, {"a", "x", "y", "c"}, {{1, 2, 1, 3}}},
        {"shrink replacement", {"a", "b", "c", "d"}, {"a", "x", "d"}, {{1, 3, 1, 2}}},
        {"separate edits",
         {"a", "b", "c", "d", "e"},
         {"x", "b", "y", "d", "z"},
         {{0, 1, 0, 1}, {2, 3, 2, 3}, {4, 5, 4, 5}}},
        {"shifted offsets", {"a", "b", "c", "d", "e"}, {"a", "x", "y", "b", "c", "e"}, {{1, 1, 1, 3}, {3, 4, 5, 5}}},
        {"one base line matched", {"b"}, {"a", "b", "c"}, {{0, 0, 0, 1}, {1, 1, 2, 3}}},
        {"one side line matched", {"a", "b", "c"}, {"b"}, {{0, 1, 0, 0}, {2, 3, 1, 1}}},
        {"repeated lines", {"a", "a", "b", "a"}, {"a", "b", "a"}, {{1, 2, 1, 1}}},
        {"case and whitespace", {"abc", " abc", ""}, {"ABC", "abc ", ""}, {{0, 2, 0, 2}}},
        {"binary line", {std::string("a\0b", 3)}, {std::string("a\0c", 3)}, {{0, 1, 0, 1}}},
    };
    for (const Expected& expected : tests) {
        if (!compare(::diff(expected.base, expected.side), expected.edits)) {
            std::cout << "DIFF TEST FAILURE: " << expected.name << '\n';
            out = false;
        }
    }
    return out;
}