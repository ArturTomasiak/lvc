#include <core.hpp>

// TREE PATTERN
// <type> <id> <name>

using ObjectMap  = std::unordered_map<std::filesystem::path, std::string>;
using VersionMap = std::unordered_map<std::filesystem::path, object::info>;

struct VersionObject {
    std::string type;
    std::string id;
    std::string name;
};

struct Tree {
    std::filesystem::path                             relative;
    std::vector<VersionObject>                        objects;
    std::unordered_map<std::filesystem::path, size_t> objects_map;
};

struct TreeRelation {
    std::string               name;
    Tree                      tree;
    std::vector<TreeRelation> children;
};

static TreeRelation tree_relation_create(const std::string& name, const std::filesystem::path& relative) {
    TreeRelation tree_relation{.name = name, .tree = {.relative = relative}};
    tree_relation.children.reserve(PREALLOCATE_SMALL);
    tree_relation.tree.objects.reserve(PREALLOCATE_SMALL);
    tree_relation.tree.objects_map.reserve(PREALLOCATE_SMALL);
    return tree_relation;
}

static void add_object(
    const std::filesystem::path& path, const std::filesystem::path& object_dir, Tree& tree, ObjectMap& object_map, char** error_message) {
    std::string    filename = path.filename();
    VersionObject* object;
    const auto     it = tree.objects_map.find(filename);
    if (it != tree.objects_map.end())
        object = &tree.objects[it->second];
    else {
        tree.objects_map.insert({filename, tree.objects.size()});
        object = &tree.objects.emplace_back();
    }
    object->name = filename;
    object->type = TYPE_BLOB;

    const auto object_it = object_map.find(path);
    if (object_it == object_map.end()) {
        std::string buffer = io::content(path, 0, 0, error_message);
        object::create(object_dir, TYPE_BLOB, buffer, object->id, error_message);
    } else {
        object->id = object_it->second;
    }
}

static void rem_object(const std::filesystem::path& filename, Tree& tree) {
    const auto it = tree.objects_map.find(filename);
    if (it != tree.objects_map.end()) {
        size_t i = it->second;
        tree.objects_map.erase(it);
        tree.objects.erase(tree.objects.begin() + i);
        for (auto& [name, index] : tree.objects_map)
            if (index > i)
                index--;
    }
}

static std::string tree_builder(TreeRelation& relation, const std::filesystem::path& object_dir, char** error_message) {
    std::string tree_content;
    std::string type;
    std::string id;
    std::string name;

    type = TYPE_TREE;
    for (TreeRelation& child : relation.children) {
        id   = tree_builder(child, object_dir, error_message);
        name = child.tree.relative.filename();
        tree_content += type + " " + id + " " + name + "\n";
    }

    type = TYPE_BLOB;
    for (VersionObject& object : relation.tree.objects) {
        id   = object.id;
        name = object.name;
        tree_content += type + " " + id + " " + name + "\n";
    }

    object::create(object_dir, TYPE_TREE, tree_content, id, error_message);
    return id;
}

static void tree_from_version(
    const std::filesystem::path& object_dir, TreeRelation& tree_relation, const std::string& tree_id, std::filesystem::path current,
    char** error_message) {
    std::vector<std::string> tree_content = io::content_lines(object_dir / tree_id, 1, 0, error_message);
    size_t                   pos;
    tree_relation.tree.relative = current;
    for (size_t i = 1; i < tree_content.size(); ++i) {
        std::string   line = tree_content[i];
        VersionObject object;
        pos         = line.find(' ');
        object.type = line.substr(0, pos);

        line = line.substr(pos + 1);
        pos  = line.find(' ');

        object.id   = line.substr(0, pos);
        object.name = line.substr(pos + 1);

        if (object.type == TYPE_TREE) { // tree objects are inserted in tree_builder
            tree_from_version(
                object_dir, tree_relation.children.emplace_back(tree_relation_create(object.name, current)), object.id,
                current / object.name, error_message);
        } else {
            tree_relation.tree.objects_map.insert({object.name, tree_relation.tree.objects.size()});
            tree_relation.tree.objects.push_back(object);
        }
    }
}

static TreeRelation& ensure_tree(TreeRelation& root, const std::filesystem::path& relative, std::filesystem::path::iterator it) {
    TreeRelation* parent = &root;
    for (; it != relative.end(); it++) {
        const std::string          name     = it->string();
        std::vector<TreeRelation>& children = parent->children;

        auto child = std::find_if(children.begin(), children.end(), [&](const TreeRelation& entry) { return entry.name == name; });

        if (child == children.end()) {
            rem_object(name, parent->tree);
            parent = &children.emplace_back(tree_relation_create(name, parent->tree.relative / *it));
        } else {
            parent = &*child;
        }
    }
    return *parent;
}

static void
remove_subtree(TreeRelation& tree_relation, const std::filesystem::path& relative, std::filesystem::path::iterator relative_it) {
    const std::filesystem::path parent = relative.parent_path();
    const std::filesystem::path name   = relative.filename();

    if (tree_relation.tree.relative == parent) {
        rem_object(name, tree_relation.tree);
    }

    for (auto it = tree_relation.children.begin(); it != tree_relation.children.end(); it++) {
        if (it->tree.relative == relative) {
            tree_relation.children.erase(it);
            return;
        } else if (it->name == relative_it->string()) {
            remove_subtree(*it, relative, std::next(relative_it));
            return;
        }
    }
}

static std::string build(
    const std::filesystem::path& object_dir, const std::filesystem::path& working_dir, const std::string& tree_id,
    const std::vector<std::string>& status, ObjectMap& object_map, char** error_message) {
    TreeRelation tree_relation = tree_relation_create({}, {});

    if (!tree_id.empty())
        tree_from_version(object_dir, tree_relation, tree_id, {}, error_message);

    for (std::string relative_str : status) {
        if (relative_str.starts_with(PREFIX_DELETED))
            relative_str.erase(0, strlen(PREFIX_DELETED));

        const std::filesystem::path relative{relative_str};
        const std::filesystem::path path = working_dir / relative;

        const std::filesystem::file_status file_status = std::filesystem::status(path);
        if (file_status.type() == std::filesystem::file_type::directory) {
            ensure_tree(tree_relation, relative, relative.begin());
        } else if (file_status.type() == std::filesystem::file_type::regular) {
            const std::filesystem::path parent_path = relative.parent_path();
            TreeRelation&               parent      = ensure_tree(tree_relation, parent_path, parent_path.begin());
            if (!tree_id.empty()) {
                const std::string name = relative.filename().string();
                std::erase_if(parent.children, [&](const TreeRelation& child) { return child.name == name; });
            }
            add_object(path, object_dir, parent.tree, object_map, error_message);
        } else if (file_status.type() == std::filesystem::file_type::not_found) {
            remove_subtree(tree_relation, relative, relative.begin());
        }
    }

    return tree_builder(tree_relation, object_dir, error_message);
}

static std::vector<std::string> process_input(
    Paths& paths, const std::vector<std::string>& input_raw, const std::string& version_id, char** error_message, ObjectMap& object,
    VersionMap& version) {
    version = version::map_all_objects(paths, version_id, {}, error_message);
    std::vector<std::string>       input  = paths.resolve(input_raw, version_id, 1, 1, (void*)&version);
    const std::vector<std::string> ignore = paths.resolve(io::content_lines(paths.ignore, 0, 0, error_message), version_id, 0, 1, (void*)&version);

    std::unordered_set<std::string_view> ignore_entries;
    ignore_entries.reserve(ignore.size());
    for (const std::string& entry : ignore)
        ignore_entries.insert(entry);
    std::erase_if(input, [&ignore_entries](const std::string& entry) { return ignore_entries.contains(entry); });

    std::erase_if(input, [&object, &version, &paths, &error_message](std::string& entry) {
        const auto it = version.find(entry);
        if (it == version.end() || it->second.type != object::type::blob)
            return false;

        const std::filesystem::path path = paths.root / entry;
        if (!std::filesystem::is_regular_file(path))
            return false;
        std::string buffer = io::content(path, 0, 0, error_message);
        std::string id;
        object::create(paths.object, TYPE_BLOB, buffer, id, error_message);
        object.insert({entry, id});
        return id == it->second.id;
    });

    return input;
}

static bool confirm_input(Paths& paths, const std::vector<std::string>& input, VersionMap& version_map, const std::string& branch_name) {
    std::cout << "Snapshot contents:\n";
    for (const std::string path : input) {
        const auto status = std::filesystem::status(paths.root / path);
        const auto it     = version_map.find(path);
        if (status.type() == std::filesystem::file_type::regular)
            std::cout << path << '\n';
        else if (status.type() == std::filesystem::file_type::not_found && it != version_map.end() && it->second.type == object::type::blob)
            std::cout << "DELETED " << path << '\n';
    }
    while (true) {
        std::string answer;
        std::cout << "{" << branch_name << "} Confirm snapshot (Y/N): ";
        std::getline(std::cin, answer);
        const char charanswer = std::tolower(answer[0]);
        if (charanswer == 'y')
            return true;
        else if (charanswer == 'n')
            return false;
        std::cout << "Invalid answer.\n";
    }
}

void version::create(
    Paths& paths, const std::vector<std::string>& input_raw, std::string message, std::string author, char** error_message) {
    ObjectMap                      object_map;
    VersionMap                     version_map;
    const std::string              branch_name    = io::content(paths.current, 0, 0, error_message);
    const std::filesystem::path    branch         = paths.branch / branch_name;
    const std::vector<std::string> branch_content = io::content_lines(branch, 1, 0, error_message);
    const std::string              version_id     = branch_content.size() <= 1 ? std::string{} : branch_content[1];
    const std::string              category       = category::get(branch_content[0]);
    const std::vector<std::string> input          = process_input(paths, input_raw, version_id, error_message, object_map, version_map);
    const std::filesystem::path    local          = paths.local / branch_name;
    const std::filesystem::path    merge          = local / filename_merge;
    const std::filesystem::path    conflict       = local / filename_conflict;
    const std::string merge_buffer = std::filesystem::is_regular_file(merge) ? io::content(merge, 0, 0, error_message) : std::string{};

#if CONFIRM_VERSION_INPUT == true
    if (!confirm_input(paths, input, version_map, branch_name))
        return;
#endif

    if (!operation::conflicts_handled(paths.root, conflict, error_message)) {
        error_message_creator("Unhandled conflicts. Use lvc conflict for the list of conflicts.", error_message);
        return;
    }
    std::string root_id;
    if (!version_id.empty()) {
        std::vector<std::string> version_content = io::content_lines(paths.object / version_id, 1, 0, error_message);
        if (version_content.empty()) {
            error_message_creator("VERSION CREATION FAILURE: failed to read latest version, ID: " + version_id, error_message);
            return;
        }
        root_id = version_content[VERSION_ROOT_TREE];
    }

    std::string       buffer = build(paths.object, paths.root, root_id, input, object_map, error_message);
    const std::string nl     = "\n";
    buffer += nl + message + nl + author;
    if (!merge_buffer.empty()) {
        buffer += nl + merge_buffer;
    }

    const Pending pending = {.type = PendingType::rename, .src = paths.get_tmp_file(), .dest = branch};

    std::string new_version;
    object::create(paths.object, TYPE_VERSION, buffer, new_version, error_message);

    std::string new_branch;
    new_branch.reserve(65 * (branch_content.size() + 3));
    new_branch = new_version + '\n';
    for (size_t i = 1; i < branch_content.size(); i++)
        new_branch += branch_content[i] + '\n';
    insert_pattern(new_branch, TYPE_BRANCH, category.empty() ? nullptr : category.c_str());

    io::file(pending.src, std::ios::binary | std::ios::trunc, new_branch.data(), new_branch.size(), 1, error_message);

    CHECKERROR(error_message);

    std::cout << "Successfully created version " + new_version + '\n';

    paths.move_pending(pending);
    paths.move_pending({.type = PendingType::remove_conditional_exists, .dest = merge});
    paths.move_pending({.type = PendingType::remove_conditional_exists, .dest = conflict});
}

void version::create_tmp(
    Paths& paths, const std::filesystem::path& operation, const std::unordered_set<std::string_view>& ignore, char** error_message) {
    const Pending               pending    = {.type = PendingType::rename, .src = paths.get_tmp_file(), .dest = operation / filename_tmp};
    const std::filesystem::path object_dir = operation / foldername_object;
    ObjectMap                   object_map;
    object_map.reserve(PREALLOCATE);

    if (!std::filesystem::is_directory(object_dir))
        std::filesystem::create_directories(object_dir);

    std::vector<std::string> status;
    try {
        std::filesystem::recursive_directory_iterator iterator(paths.root);
        for (const std::filesystem::directory_entry& entry : iterator) {
            if (entry.is_directory() && entry.path().filename() == ".lvc") {
                iterator.disable_recursion_pending();
                continue;
            }
            std::filesystem::path relative = entry.path().lexically_relative(paths.root);
            if (ignore.contains(relative.string())) {
                if (entry.is_directory())
                    iterator.disable_recursion_pending();

                continue;
            }
            status.emplace_back(relative);
        }
    } catch (const std::filesystem::filesystem_error& error) {
        error_message_creator("Directory iteration failure", error_message);
        return;
    }

    std::string root_id;
    std::string new_root_id = build(object_dir, paths.root, root_id, status, object_map, error_message);

    const std::string nl     = "\n";
    std::string       buffer = new_root_id + nl + "tmp snapshot" + nl + "lvc";

    std::string id;
    object::create(object_dir, TYPE_VERSION, buffer, id, error_message);
    io::file(pending.src, std::ios::binary | std::ios::trunc, id.data(), id.size(), 0, error_message);
    paths.move_pending(pending);
}