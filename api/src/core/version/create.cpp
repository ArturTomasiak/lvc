#include <core.hpp>

// TREE PATTERN
// <type> <id> <name>

struct VersionObject {
    std::string type;
    std::string id;
    std::string name;
};

struct Tree {
    std::filesystem::path      relative;
    std::vector<VersionObject> objects;
};

struct TreeRelation {
    Tree                      tree;
    std::vector<TreeRelation> children;
};

static void add_object(
    const std::filesystem::path& path, const std::filesystem::path& object_dir, std::vector<VersionObject>& objects,
    char** error_message) {
    std::string filename = path.filename();
    std::erase_if(objects, [&](const VersionObject& object) { return object.name == filename; });
    VersionObject object;
    object.name        = filename;
    object.type        = TYPE_BLOB;
    std::string buffer = io::content(path, 0, 0, error_message);
    object::create(object_dir, TYPE_BLOB, buffer, object.id, error_message);
    objects.push_back(std::move(object));
}

static void rem_object(const std::filesystem::path& path, std::vector<VersionObject>& objects) {
    std::erase_if(objects, [&](const VersionObject& object) { return object.name == path.filename(); });
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

static void
tree_relation_builder(std::vector<Tree>& trees, TreeRelation& tree_relation, std::filesystem::path current) {
    std::vector<Tree>::iterator iterator = trees.begin();
    while (iterator != trees.end()) {
        std::filesystem::path tree_parent = iterator->relative.parent_path();
        if (tree_parent == current) {
            tree_relation.children.push_back({std::move(*iterator), {}});
            iterator = trees.erase(iterator);
            tree_relation_builder(trees, tree_relation.children.back(), tree_relation.children.back().tree.relative);
            iterator = trees.begin();
        } else
            iterator++;
    }
}

static TreeRelation tree_relation_init(std::vector<Tree>& trees) {
    TreeRelation tree_relation;
    for (std::vector<Tree>::iterator iterator = trees.begin(); iterator != trees.end(); iterator++) {
        if (iterator->relative.empty()) {
            tree_relation.tree = std::move(*iterator);
            trees.erase(iterator);
            break;
        }
    }
    tree_relation_builder(trees, tree_relation, {});
    return tree_relation;
}

static void tree_from_version(
    const std::filesystem::path& object_dir, std::vector<Tree>& trees, const std::string& tree_id,
    std::filesystem::path current, char** error_message) {
    std::vector<std::string> tree_content = io::content_lines(object_dir / tree_id, 1, 0, error_message);
    size_t                   pos;
    Tree                     tree;
    tree.relative = current;
    for (size_t i = 1; i < tree_content.size(); ++i) {
        std::string   line = tree_content[i];
        VersionObject object;
        pos         = line.find(' ');
        object.type = line.substr(0, pos);

        line = line.substr(pos + 1);
        pos  = line.find(' ');

        object.id   = line.substr(0, pos);
        object.name = line.substr(pos + 1);

        if (object.type == TYPE_TREE) // tree objects are inserted in tree_builder
            tree_from_version(object_dir, trees, object.id, current / object.name, error_message);
        else
            tree.objects.push_back(object);
    }
    trees.push_back(tree);
}

static Tree& ensure_tree(std::vector<Tree>& trees, const std::filesystem::path& relative) {
    for (Tree& tree : trees)
        if (tree.relative == relative)
            return tree;

    if (!relative.empty()) {
        Tree& parent = ensure_tree(trees, relative.parent_path());
        std::erase_if(parent.objects, [&](const VersionObject& object) { return object.name == relative.filename(); });
    }
    return trees.emplace_back(Tree{.relative = relative});
}

static void remove_subtree(std::vector<Tree>& trees, const std::filesystem::path& relative) {
    std::erase_if(trees, [&](const Tree& tree) {
        return std::mismatch(relative.begin(), relative.end(), tree.relative.begin(), tree.relative.end()).first ==
               relative.end();
    });
}

static std::string build(
    const std::filesystem::path& object_dir, const std::filesystem::path& working_dir, const std::string& tree_id,
    const std::vector<std::string>& status, char** error_message) {
    std::vector<Tree> trees;
    trees.reserve(PREALLOCATE);

    if (!tree_id.empty())
        tree_from_version(object_dir, trees, tree_id, {}, error_message);
    else
        trees.emplace_back();

    for (std::string relative_str : status) {
        if (relative_str.starts_with(PREFIX_DELETED))
            relative_str.erase(0, strlen(PREFIX_DELETED));

        const std::filesystem::path relative{relative_str};
        const std::filesystem::path path = working_dir / relative;

        const auto file_status = std::filesystem::status(path);
        if (file_status.type() == std::filesystem::file_type::directory) {
            ensure_tree(trees, relative);
        } else if (file_status.type() == std::filesystem::file_type::regular) {
            if (!tree_id.empty())
                remove_subtree(trees, relative);
            Tree& parent = ensure_tree(trees, relative.parent_path());
            add_object(path, object_dir, parent.objects, error_message);
        } else if (file_status.type() == std::filesystem::file_type::not_found) {
            remove_subtree(trees, relative);
            for (Tree& tree : trees)
                if (tree.relative == relative.parent_path())
                    rem_object(relative, tree.objects);
        }
    }

    TreeRelation tree_relation = tree_relation_init(trees);

    return tree_builder(tree_relation, object_dir, error_message);
}

void version::create(Paths& paths, std::string message, std::string author, char** error_message) {
    if (message.empty()) {
        error_message_creator("No version message recieved", error_message);
        return;
    }
    const std::string              workspace_name    = io::content(paths.current, 0, 0, error_message);
    const std::filesystem::path    workspace         = paths.workspace / workspace_name;
    const std::vector<std::string> workspace_content = io::content_lines(workspace, 1, 0, error_message);
    const std::filesystem::path    prepare_dir       = prepare_path(paths, workspace_name);
    const std::string              version_id = workspace_content.size() <= 1 ? std::string{} : workspace_content[1];
    const std::string              category   = category::get(workspace_content[0]);
    if (!std::filesystem::is_regular_file(prepare_dir)) [[unlikely]] {
        error_message_creator("No files prepared", error_message);
        return;
    }
    const std::filesystem::path local    = paths.local / workspace_name;
    const std::filesystem::path merge    = local / filename_merge;
    const std::filesystem::path conflict = local / filename_conflict;
    const std::string           merge_buffer =
        std::filesystem::is_regular_file(merge) ? io::content(merge, 0, 0, error_message) : std::string{};

    if (!operation::conflicts_handled(paths.root, conflict, error_message)) {
        error_message_creator("Unhandled conflicts. Use lvc conflict for the list of conflicts.", error_message);
        return;
    }
    std::vector<std::string> status = version::status(paths, prepare_dir, version_id, error_message);
    std::string              root_id;
    if (!version_id.empty()) {
        std::vector<std::string> version_content = io::content_lines(paths.object / version_id, 1, 0, error_message);
        if (version_content.empty()) {
            error_message_creator(
                "VERSION CREATION FAILURE: failed to read latest version, ID: " + version_id, error_message);
            return;
        }
        root_id = version_content[VERSION_ROOT_TREE];
    }

    std::string       buffer = build(paths.object, paths.root, root_id, status, error_message);
    const std::string nl     = "\n";
    buffer += nl + message + nl + author;
    if (!merge_buffer.empty()) {
        buffer += nl + merge_buffer;
    }

    const Pending pending = {.type = PendingType::rename, .src = paths.get_tmp_file(), .dest = workspace};

    std::string new_workspace;
    new_workspace.reserve(65 * (workspace_content.size() + 3));
    object::create(paths.object, TYPE_VERSION, buffer, new_workspace, error_message);
    new_workspace += '\n';
    for (size_t i = 1; i < workspace_content.size(); i++)
        new_workspace += workspace_content[i] + '\n';
    insert_pattern(new_workspace, TYPE_WORKSPACE, category.empty() ? nullptr : category.c_str());

    io::file(
        pending.src, std::ios::binary | std::ios::trunc, new_workspace.data(), new_workspace.size(), 1, error_message);

    CHECKERROR(error_message);

    paths.move_pending(pending);
    paths.move_pending({.type = PendingType::remove, .dest = prepare_dir});
    paths.move_pending({.type = PendingType::remove_conditional_exists, .dest = merge});
    paths.move_pending({.type = PendingType::remove_conditional_exists, .dest = conflict});
}

void version::create_tmp(
    Paths& paths, const std::filesystem::path& operation, std::unordered_set<std::string_view>& ignore,
    char** error_message) {
    const Pending pending = {
        .type = PendingType::rename, .src = paths.get_tmp_file(), .dest = operation / filename_tmp};
    const std::filesystem::path object_dir = operation / foldername_object;

    paths.move_pending({.type = PendingType::create_directories, .dest = object_dir});

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
    std::string new_root_id = build(object_dir, paths.root, root_id, status, error_message);

    const std::string nl     = "\n";
    std::string       buffer = new_root_id + nl + "tmp snapshot" + nl + "lvc";

    std::string id;
    object::create(object_dir, TYPE_VERSION, buffer, id, error_message);
    io::file(pending.src, std::ios::binary | std::ios::trunc, id.data(), id.size(), 0, error_message);
    paths.move_pending(pending);
}