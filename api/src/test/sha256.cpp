#include <test.hpp>

struct Expected {
    std::string input;
    std::string output;
};

static bool compare(const Expected& expected) {
    char output[65];
    sha256(expected.input.data(), expected.input.size(), output);
    if (strcmp(output, expected.output.c_str()))
        return true;
    return false;
}

void test::sha256() {
    const std::vector<Expected> tests = {
        // 1-8: Case sensitivity, whitespace, special characters, empty
        {"", "e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855"},
        {"ABC", "b5d4045c3f466fa91fe2cc6abe79232a1a57cdf104f7a26e716e0a1e2789df78"},
        {"abc\n", "edeaaff3f1774ad2888673770c6d64097e391bc362d7d6fb34982ddf0efd18cb"},
        {"abc\r\n", "552bab6864c7a7b69a502ed1854b9245c0e1a30f008aaa0b281da62585fdb025"},
        {" abc ", "3eaf1941003943dfaa935adecffcaaa217e290def6fb0181141ced6c9daabaad"},
        {"0", "5feceb66ffc86f38d952786c6d696c79c2dbc239dd4e91b46729d73a27fb57e9"},
        {"1234567890", "c775e7b757ede630cd0aa1113bd102661ab38829ca52a6422ab782862f268646"},
        {"!@#$%^&*()", "95ce789c5c9d18490972709838ca3a9719094bca3ac16332cfec0652b0236141"},
        // 9-14: Binary data, null bytes, high-bit values
        {std::string(1, '\0'), "6e340b9cffb37a989ca544e6bb780a2c78901d3fb33738768511a30617afa01d"},
        {std::string("a\0b", 3), "59b271ae1bbcb1d31d41929817f4b16fb439eb4f31520b5ad1d5ce98920a7138"},
        {std::string(4, '\0'), "df3f619804a92fdb4057192dc43dd748ea778adc52bc498ce80524c014b81119"},
        {std::string(1, '\xff'), "a8100ae6aa1940d0b663bb31cd466142ebbdbd5187131b92d93818987832eb89"},
        {std::string("\x00\x01\x7f\x80\xff", 5), "0150a92bb1212cd00516b65fde0704614760000963874fcbb11eaa734ee87809"},
        // 15: Large input
        {std::string(9999, 'a'), "5e239d7f94d775403bb340918f842229d6fa59b49759998c64bb198296edc1bc"},
    };
    for (const Expected& expected : tests) {
        if (compare(expected) == false)
            std::cout << "SHA256 TEST FAILURE: " << expected.input << '\n';
    }
}