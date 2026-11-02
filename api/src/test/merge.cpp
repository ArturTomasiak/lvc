#include <chrono>
#include <fstream>
#include <iterator>
#include <test.hpp>

struct ExpectedMerge {
    Lines       base, ours, theirs;
    std::string output;
    bool        conflict;
};

static bool compare(const std::filesystem::path& path, const ExpectedMerge& expected) {
    const bool    conflict = merge3(path, expected.base, expected.ours, expected.theirs);
    std::ifstream file(path, std::ios::binary);
    if (!file.is_open())
        return false;
    const std::string output((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    return !file.bad() && conflict == expected.conflict && output == expected.output;
}

bool test::merge() {
    bool                             out   = true;
    const std::vector<ExpectedMerge> tests = {
        {{}, {}, {}, "", false},
        {{"a", "b"}, {"a", "b"}, {"a", "b"}, "a\nb\n", false},
        {{"a", "b", "c"}, {"a", "x", "c"}, {"a", "b", "c"}, "a\nx\nc\n", false},
        {{"a", "b", "c"}, {"a", "b", "c"}, {"a", "x", "c"}, "a\nx\nc\n", false},
        {{}, {"a", "b"}, {}, "a\nb\n", false},
        {{}, {}, {"a", "b"}, "a\nb\n", false},
        {{}, {"a", "b"}, {"a", "b"}, "a\nb\n", false},
        {{}, {"x"}, {"y"}, "<<<  OURS  <<<\nx\n>>> THEIRS >>>\ny\n==============\n", true},
        {{"a", "b"}, {}, {"a", "b"}, "", false},
        {{"a", "b"}, {"a", "b"}, {}, "", false},
        {{"a", "b"}, {}, {}, "", false},
        {{"a", "b", "c"}, {"a", "x", "c"}, {"a", "x", "c"}, "a\nx\nc\n", false},
        {{"a", "b", "c"}, {"a", "c"}, {"a", "c"}, "a\nc\n", false},
        {{"a", "b", "c"}, {"x", "b", "c"}, {"a", "b", "y"}, "x\nb\ny\n", false},
        {{"a", "b"}, {"x", "b"}, {"a", "y"}, "x\ny\n", false},
        {{"a"}, {"x", "a"}, {"a", "y"}, "x\na\ny\n", false},
        {{"a", "b", "c", "d", "e"},
         {"a", "x", "y", "b", "c", "e"},
         {"a", "b", "z", "d", "e", "w"},
         "a\nx\ny\nb\nz\ne\nw\n",
         false},
        {{"a", "b", "c"},
         {"a", "x", "c"},
         {"a", "y", "c"},
         "a\n<<<  OURS  <<<\nx\n>>> THEIRS >>>\ny\n==============\nc\n",
         true},
        {{"a", "b"},
         {"a", "x", "b"},
         {"a", "y", "b"},
         "a\n<<<  OURS  <<<\nx\n>>> THEIRS >>>\ny\n==============\nb\n",
         true},
        {{"a"}, {"a", "x"}, {"a", "y"}, "a\n<<<  OURS  <<<\nx\n>>> THEIRS >>>\ny\n==============\n", true},
        {{"a"}, {}, {"x"}, "<<<  OURS  <<<\n>>> THEIRS >>>\nx\n==============\n", true},
        {{"a"}, {"x"}, {}, "<<<  OURS  <<<\nx\n>>> THEIRS >>>\n==============\n", true},
        {{"a", "b", "c", "d", "e"},
         {"a", "x", "d", "e"},
         {"a", "b", "y", "e"},
         "a\n<<<  OURS  <<<\nx\nd\n>>> THEIRS >>>\nb\ny\n==============\ne\n",
         true},
        {{"a", "b", "c", "d"},
         {"a", "d"},
         {"a", "b", "x", "c", "d"},
         "a\n<<<  OURS  <<<\n>>> THEIRS >>>\nb\nx\nc\n==============\nd\n",
         true},
        {{"a", "b"}, {"x", "a", "b"}, {"y", "b"}, "x\ny\nb\n", false},
        {{"a", "b"}, {"y", "b"}, {"x", "a", "b"}, "x\ny\nb\n", false},
        {{"a", "b"}, {"x", "b"}, {"a", "y", "b"}, "x\ny\nb\n", false},
        {{"a", "b", "c"},
         {"x", "b", "u"},
         {"y", "b", "v"},
         "<<<  OURS  <<<\nx\n>>> THEIRS >>>\ny\n==============\nb\n<<<  OURS  <<<\nu\n>>> THEIRS "
         ">>>\nv\n==============\n",
         true},
        {{"a", "b", "c", "d", "e", "f", "g"},
         {"x", "c", "y", "g"},
         {"a", "z", "f", "g"},
         "<<<  OURS  <<<\nx\nc\ny\n>>> THEIRS >>>\na\nz\nf\n==============\ng\n",
         true},
        {{"abc", "", "tail"}, {"ABC", "", "tail"}, {"abc", "", "tail "}, "ABC\n\ntail \n", false},
        {{std::string("a\0b", 3)}, {std::string("a\0c", 3)}, {std::string("a\0b", 3)}, std::string("a\0c\n", 4), false},
    };

    std::error_code             error;
    const std::filesystem::path root = std::filesystem::temp_directory_path(error);
    if (error) {
        std::cout << "MERGE TEST FAILURE: could not create temporary directory\n";
        return false;
    }
    const std::chrono::nanoseconds::rep stamp = std::chrono::system_clock::now().time_since_epoch().count();
    const std::filesystem::path         path  = root / ("lvc-test-merge-" + std::to_string(stamp));
    std::filesystem::create_directory(path, error);
    if (path.empty() || error) {
        std::cout << "MERGE TEST FAILURE: could not create temporary directory\n";
        return false;
    }
    for (const ExpectedMerge& expected : tests) {
        if (!compare(path / "output", expected)) {
            std::cout << "MERGE TEST FAILURE:\nBASE:\n";
            print_lines(expected.base);
            std::cout << "OURS:\n";
            print_lines(expected.ours);
            std::cout << "THEIRS:\n";
            print_lines(expected.theirs);
            out = false;
        }
    }
    std::filesystem::remove_all(path);
    return out;
}
