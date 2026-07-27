#include <core.hpp>

struct ProcessedInput {
    bool root      = false;
    bool exclude   = false;
    bool directory = false;
    std::vector<std::string> path_components;
};

static std::vector<ProcessedInput> process_input(const std::vector<std::string>& inputs, bool& all) {
    std::vector<ProcessedInput> result;
    result.reserve(inputs.size());
    std::string component;
    std::stringstream stream;
    for (std::string input : inputs) {
        if (input.empty() || input == "!")
            continue;
        ProcessedInput processed;
        if (input.front() == '!') {
            processed.exclude = true;
            input.erase(0, 1);
        }
        if (input == ".") {
            result.clear();
            result.emplace_back();
            result.back().path_components.emplace_back("*");
        }
        processed.root      = input.front() == '/';
        processed.directory = input.back()  == '/';
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
    constexpr uint64_t no_star = std::string_view::npos;

    uint64_t pattern_pos = 0;
    uint64_t text_pos    = 0;
    uint64_t star_pos    = no_star;
    uint64_t retry_pos   = 0;

    while (text_pos < text.size()) {
        if (pattern_pos < pattern.size() && (pattern[pattern_pos] == '?' || pattern[pattern_pos] == text[text_pos])) {
            pattern_pos++;
            text_pos++;
        } 
        else if (pattern_pos < pattern.size() && pattern[pattern_pos] == '*') {
            star_pos  = pattern_pos++;
            retry_pos = text_pos;
        } 
        else if (star_pos != no_star) {
            pattern_pos = star_pos + 1; 
            text_pos    = retry_pos++;
        } 
        else {
            return false;
        }
    }

    while (pattern_pos < pattern.size() && pattern[pattern_pos] == '*')
        pattern_pos++;

    return pattern_pos == pattern.size();
}

static bool matches_target(const ProcessedInput& rule, const std::vector<std::string>& target, uint64_t target_component_count, bool target_is_directory) {
    if (rule.directory && !target_is_directory)
        return false;

    const uint64_t pattern_count = rule.path_components.size();
    if (pattern_count == 0) {
        return target_is_directory &&
               target_component_count == 0;
    }
    if (pattern_count > target_component_count)
        return false;

    uint64_t target_begin = 0;

    if (!rule.root)
        target_begin = target_component_count - pattern_count;
    else 
        if (pattern_count != target_component_count)
            return false;

    for (uint64_t i = 0; i < pattern_count; i++)
        if (!match_glob(rule.path_components[i], target[target_begin + i]))
            return false;

    return true;
}

static bool rule_selects_file(const ProcessedInput& rule, const std::vector<std::string>& file_components) {
    if (!rule.directory && matches_target(rule, file_components, file_components.size(), false))
        return true;

    for (uint64_t count = 0; count < file_components.size(); count++)
        if (matches_target(rule, file_components, count,true))
            return true;

    return false;
}

static void iterate(const std::filesystem::path& relative_path, std::vector<std::string>& result, const std::vector<ProcessedInput>& included, const std::vector<ProcessedInput>& excluded, void* object) {
    std::vector<std::string> components;
    components.reserve(PREALLOCATE_SMALL);

    for (const std::filesystem::path& component : relative_path)
        components.push_back(component.generic_string());

    const bool included_match = std::any_of(
        included.begin(),
        included.end(),
        [&](const ProcessedInput& rule) {
            return rule_selects_file(rule, components);
        }
    );

    if (included_match) {
        bool excluded_match = std::any_of(
            excluded.begin(),
            excluded.end(),
            [&](const ProcessedInput& rule) {
                return rule_selects_file(rule, components);
            }
        );

        if (!excluded_match) {
            if (object) {
                std::string id = (*((Object*)object)).id;
                result.push_back(id);
            }
            else {
                result.push_back(relative_path.generic_string());
            }
        }
    }
}

static std::vector<std::string> path_from_processed_input(std::filesystem::path repository_root, const std::vector<ProcessedInput>& included, const std::vector<ProcessedInput>& excluded, const std::string& version_id) {
    std::vector<std::string> result;

    if (included.empty())
        return result;

    std::error_code error;
    const std::filesystem::directory_options options = std::filesystem::directory_options::skip_permission_denied;
    std::filesystem::recursive_directory_iterator iterator(repository_root, options, error);
    const std::filesystem::recursive_directory_iterator end;

    if (error)
        return result;

    result.reserve(PREALLOCATE);

    while (iterator != end) {
        error.clear();
        if (!error && iterator->is_regular_file(error)) {
            const std::filesystem::path relative_path = iterator->path().lexically_relative(repository_root);
            iterate(relative_path, result, included, excluded, 0);
        }
        iterator.increment(error);
    }

    if (!version_id.empty()) {
        std::vector<Object> version_objects = version::deleted_since(repository_root / ".lvc" / NAME_OBJECT, repository_root, version_id);
        for (const Object& object : version_objects)
            iterate(object.path, result, included, excluded, (void*)(&object));
    }
    

    return result;
}

std::vector<std::string> path_from_input(std::filesystem::path repository_root, const std::vector<std::string>& inputs, const std::string& version_id) {
    std::vector<std::string> result;
    repository_root = repository_root.lexically_normal();
    std::error_code error;
    if (!std::filesystem::is_directory(repository_root, error) || error)
        return result;

    bool all = false;
    std::vector<ProcessedInput> processed = process_input(inputs, all);

    std::vector<ProcessedInput> included;
    std::vector<ProcessedInput> excluded;
    included.reserve(processed.size());
    excluded.reserve(processed.size());
    for (ProcessedInput& item : processed)
        (item.exclude ? excluded : included).push_back(std::move(item));
    
    result = path_from_processed_input(repository_root, included, excluded, version_id);

    return result;
}