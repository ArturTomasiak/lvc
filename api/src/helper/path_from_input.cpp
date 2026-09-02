#include <core.hpp>

struct ProcessedInput {
    bool                     root      = false;
    bool                     exclude   = false;
    bool                     directory = false;
    std::vector<std::string> path_components;
};

static std::vector<ProcessedInput> process_input(const std::vector<std::string>& inputs) {
    std::vector<ProcessedInput> result;
    result.reserve(inputs.size());
    std::string       component;
    std::stringstream stream;
    for (std::string input : inputs) {
        input.erase(0, input.find_first_not_of(' '));
        input.erase(input.find_last_not_of(' ') + 1);

        if (input.empty() || input == "~")
            continue;
        ProcessedInput processed;
        if (input.front() == '~') {
            processed.exclude = true;
            input.erase(0, 1);
        }
        processed.root      = input.front() == '/';
        processed.directory = input.back() == '/';
        processed.path_components.reserve(PREALLOCATE_SMALL);
        stream.clear();
        stream.str(input);
        while (std::getline(stream, component, '/')) {
            if (component.empty() || component == ".")
                continue;
            if (component == "..")
                goto invalid;
            processed.path_components.push_back(component);
        }
        result.push_back(std::move(processed));
    invalid:;
    }
    return result;
}

static bool match_glob(std::string_view pattern, std::string_view text) {
    constexpr size_t no_star = std::string_view::npos;

    size_t pattern_pos = 0;
    size_t text_pos    = 0;
    size_t star_pos    = no_star;
    size_t retry_pos   = 0;

    while (text_pos < text.size()) {
        if (pattern_pos < pattern.size() && (pattern[pattern_pos] == '?' || pattern[pattern_pos] == text[text_pos])) {
            pattern_pos++;
            text_pos++;
        } else if (pattern_pos < pattern.size() && pattern[pattern_pos] == '*') {
            star_pos  = pattern_pos++;
            retry_pos = text_pos;
        } else if (star_pos != no_star) {
            pattern_pos = star_pos + 1;
            text_pos    = retry_pos++;
        } else {
            return false;
        }
    }

    while (pattern_pos < pattern.size() && pattern[pattern_pos] == '*')
        pattern_pos++;

    return pattern_pos == pattern.size();
}

static bool
matches_target(const ProcessedInput& rule, const std::vector<std::string>& target, size_t target_component_count, bool target_is_directory) {
    const size_t pattern_count = rule.path_components.size();
    if (pattern_count == 0) {
        return target_is_directory && target_component_count == 0;
    }
    if (pattern_count > target_component_count)
        return false;

    size_t target_begin = 0;

    if (!rule.root)
        target_begin = target_component_count - pattern_count;
    else if (pattern_count != target_component_count)
        return false;

    for (size_t i = 0; i < pattern_count; i++)
        if (!match_glob(rule.path_components[i], target[target_begin + i]))
            return false;

    return true;
}

static bool rule_selects_file(const ProcessedInput& rule, const std::vector<std::string>& file_components) {
    if (matches_target(rule, file_components, file_components.size(), rule.directory))
        return true;

    for (size_t count = 0; count < file_components.size(); count++)
        if (matches_target(rule, file_components, count, true))
            return true;

    return false;
}

static void iterate(
    const std::filesystem::path& relative_path, std::vector<std::string>& result, const std::vector<ProcessedInput>& included,
    const std::vector<ProcessedInput>& excluded, bool prefix_deleted) {
    std::vector<std::string> components;
    components.reserve(PREALLOCATE_SMALL);

    for (const std::filesystem::path& component : relative_path)
        components.push_back(component.generic_string());

    const bool included_match =
        std::any_of(included.begin(), included.end(), [&](const ProcessedInput& rule) { return rule_selects_file(rule, components); });

    if (included_match) {
        bool excluded_match =
            std::any_of(excluded.begin(), excluded.end(), [&](const ProcessedInput& rule) { return rule_selects_file(rule, components); });

        if (!excluded_match) {
            std::string out;
            if (prefix_deleted)
                out += PREFIX_DELETED;
            out += relative_path.generic_string();
            result.push_back(std::move(out));
        }
    }
}

static std::vector<std::string> path_from_processed_input(
    std::filesystem::path repository_root, const std::vector<ProcessedInput>& included, const std::vector<ProcessedInput>& excluded,
    const std::string& version_id, bool prefix_deleted, char** error_message) {
    std::vector<std::string> result;

    if (included.empty())
        return result;

    result.reserve(PREALLOCATE);

    try {
        std::filesystem::recursive_directory_iterator iterator(repository_root);
        for (const std::filesystem::directory_entry& entry : iterator) {
            const std::filesystem::path relative_path = entry.path().lexically_relative(repository_root);
            iterate(relative_path, result, included, excluded, 0);
        }
    } catch (const std::filesystem::filesystem_error& error) {
        error_message_creator("Directory iteration failure", error_message);
        return {};
    }

    if (!version_id.empty()) {
        std::vector<std::filesystem::path> deleted =
            version::deleted_since(repository_root / ".lvc" / NAME_OBJECT, repository_root, version_id, error_message);
        if (*error_message)
            return {};
        for (const std::filesystem::path& path : deleted)
            iterate(path, result, included, excluded, prefix_deleted);
    }

    return result;
}

std::vector<std::string> path_from_input(
    std::filesystem::path repository_root, const std::vector<std::string>& inputs, const std::string version_id, bool prefix_deleted,
    char** error_message) {
    repository_root = repository_root.lexically_normal();
    std::error_code error;
    if (!std::filesystem::is_directory(repository_root, error) || error)
        return {};

    std::vector<ProcessedInput> processed = process_input(inputs);

    std::vector<ProcessedInput> included;
    std::vector<ProcessedInput> excluded;
    included.reserve(processed.size());
    excluded.reserve(processed.size());
    for (ProcessedInput& item : processed)
        (item.exclude ? excluded : included).push_back(std::move(item));

    excluded.push_back({1, 1, 1, {".lvc"}});

    std::vector<std::string> result = path_from_processed_input(repository_root, included, excluded, version_id, prefix_deleted, error_message);

    return result;
}