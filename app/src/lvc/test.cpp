#include <app.hpp>
#include <chrono>
#include <fstream>

#define TEST_MS_MEASURE(test, pre_msg, success_msg)                                                                    \
    do {                                                                                                               \
        std::cout << pre_msg << "\n";                                                                                  \
        const auto start = std::chrono::steady_clock::now();                                                           \
        test;                                                                                                          \
        const auto elapsed =                                                                                           \
            std::chrono::duration<double, std::milli>(std::chrono::steady_clock::now() - start).count();               \
        if (error_message)                                                                                             \
            return 0;                                                                                                  \
        std::cout << success_msg << elapsed << " ms\n\n";                                                              \
                                                                                                                       \
    } while (0)

static void create_file(const std::filesystem::path& path, const std::string& content) {
    std::ofstream stream(path, std::ios::out | std::ios::binary | std::ios::trunc);
    stream.write(content.c_str(), content.size());
    stream.close();
}

static void modify_line(const std::filesystem::path& path, size_t line_number, std::string replacement) {
    std::ifstream input(path);
    std::string   buffer;
    std::string   line;
    size_t        current_line = 0;

    while (std::getline(input, line)) {
        buffer += (current_line++ == line_number ? replacement : line);
        if (!input.eof())
            buffer += '\n';
    }

    input.close();
    std::ofstream output(path);
    output << buffer;
    output.close();
}

bool lvc::test(int argc, char* argv[]) {
    const std::string           name              = "lvctest";
    const std::string           main_workspace    = "main";
    const std::string           feature_worksapce = "feature";
    const std::string           stamp = std::to_string(std::chrono::system_clock::now().time_since_epoch().count());
    const std::filesystem::path tmp   = std::filesystem::temp_directory_path() / (name + stamp);
    const std::filesystem::path lvc   = tmp / ".lvc";
    std::filesystem::create_directories(tmp);

    const std::string example_file_1 =
        "Lorem ipsum dolor sit amet, consectetur adipiscing elit. Nam justo tellus, consequat a sagittis vitae, "
        "volutpat a magna. Proin magna nisl, venenatis in dui sed, interdum sodales dui. Sed et tempus nulla. "
        "Pellentesque rhoncus purus nec nunc eleifend, eget fringilla odio congue. Etiam vel commodo enim. Quisque "
        "venenatis, augue ut vestibulum aliquet, eros elit suscipit lectus, vel ultricies neque nunc varius nibh. "
        "Nulla ultricies rutrum purus, quis congue dolor ullamcorper a. Maecenas euismod vel lectus eget "
        "fermentum.\nDonec sagittis ultrices lacus eu congue. Aliquam ultricies consequat tincidunt. Nunc elementum, "
        "ex quis elementum iaculis, libero eros suscipit erat, quis auctor ex neque eget lacus. Quisque et faucibus "
        "turpis. Nullam eleifend justo sollicitudin ex placerat convallis. Nulla rhoncus consectetur metus quis "
        "aliquam. Phasellus viverra ornare pulvinar. Lorem ipsum dolor sit amet, consectetur adipiscing elit. Sed "
        "lobortis mollis velit in egestas. Sed sit amet elit commodo, molestie ex id, sodales urna.\nPhasellus vitae "
        "consectetur justo, sed gravida nisi. Mauris lobortis, felis eu convallis suscipit, felis dolor facilisis ex, "
        "id posuere est diam a arcu. In porta felis vel dui faucibus venenatis. Phasellus vel lacus vitae orci "
        "tincidunt accumsan. Curabitur fringilla magna velit, ut scelerisque felis venenatis in. Cras molestie leo id "
        "lorem pulvinar, nec dignissim arcu dignissim. Aliquam erat volutpat. Curabitur vel felis eu risus rhoncus "
        "pretium. Aliquam blandit eros lorem. Praesent at nulla quis ex suscipit semper. In hac habitasse platea "
        "dictumst. Ut et mollis est. Integer consectetur mi non nisl dignissim, non placerat velit faucibus. Quisque "
        "molestie ac ante sed dapibus. Aliquam tempus vitae metus eget ultricies.";
    const std::string example_file_1_name = "LoremIpsum";
    const std::string example_file_1_path = tmp / example_file_1_name;
    const std::string example_file_2      = "a\nb\nc\nd\ne\nf\ng";
    const std::string example_file_2_name = "abc";
    const std::string example_file_2_path = tmp / example_file_2_name;
    const std::string example_file_3      = "Hello world";
    const std::string example_file_3_name = "test";
    const std::string example_file_3_path = tmp / example_file_3_name;
    char              prepare_input[]     = "/";
    int               argc_example        = 3;
    char*             argv_example[]      = {prepare_input, prepare_input, prepare_input};

    LvcCreateInput input{};
    input.clone_repository = 0;
    input.local            = 1;
    input.location         = tmp.c_str();
    input.repository_name  = name.c_str();
    input.workspace_name   = main_workspace.c_str();

    TEST_MS_MEASURE(lvc_run_tests(), "DIFF, MERGE AND SHA256 TESTS", "DIFF, MERGE AND SHA256 SUCCESS IN: ");
    TEST_MS_MEASURE(lvc_create(input, &error_message), "LVC CREATE TEST", "LVC CREATE SUCCESS IN: ");

    create_file(example_file_1_path, example_file_1);
    create_file(example_file_2_path, example_file_2);
    create_file(example_file_3_path, example_file_3);

    TEST_MS_MEASURE(
        lvc_prepare(lvc.c_str(), argc_example, argv_example, &error_message), "LVC PREPARE TEST",
        "LVC PREPARE SUCCESS IN: ");
    TEST_MS_MEASURE(
        lvc_version(lvc.c_str(), "init", "", &error_message), "LVC VERSION TEST", "LVC VERSION SUCCESS IN: ");
    TEST_MS_MEASURE(
        lvc_workspace(lvc.c_str(), feature_worksapce.c_str(), 0, &error_message), "LVC WORKSPACE TEST",
        "LVC WORKSPACE SUCCESS IN: ");
    TEST_MS_MEASURE(
        lvc_goto(lvc.c_str(), feature_worksapce.c_str(), &error_message), "LVC GOTO TEST", "LVC GOTO SUCCESS IN: ");

    modify_line(example_file_2_path, 2, "q");
    lvc_prepare(lvc.c_str(), argc_example, argv_example, &error_message);
    lvc_version(lvc.c_str(), "q", "", &error_message);
    lvc_goto(lvc.c_str(), main_workspace.c_str(), &error_message);
    modify_line(example_file_2_path, 3, "f");
    lvc_prepare(lvc.c_str(), argc_example, argv_example, &error_message);
    lvc_version(lvc.c_str(), "f", "", &error_message);

    TEST_MS_MEASURE(
        lvc_merge(lvc.c_str(), feature_worksapce.c_str(), &error_message), "LVC MERGE TEST", "LVC MERGE SUCCESS IN: ");

    lvc_prepare(lvc.c_str(), argc_example, argv_example, &error_message);
    lvc_version(lvc.c_str(), "merge", "", &error_message);

    TEST_MS_MEASURE(
        lvc_history(lvc.c_str(), main_workspace.c_str(), &error_message), "LVC HISTORY TEST",
        "LVC HISTORY SUCCESS IN: ");
    TEST_MS_MEASURE(
        lvc_change_category(lvc.c_str(), main_workspace.c_str(), "main", &error_message), "LVC CATEGORY TEST",
        "LVC CHANGE CATEGORY SUCCESS IN: ");
    TEST_MS_MEASURE(lvc_category_list(lvc.c_str(), &error_message), "LVC LIST TEST", "LVC CATEGORY LIST SUCCESS IN: ");

    std::filesystem::remove_all(tmp);
    return 1;
}