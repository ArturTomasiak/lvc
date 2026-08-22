#include <core.hpp>

// TREE PATTERN
// <type> <id> <name>

struct VersionObject {
    std::string type;
    std::string id;
    std::string name;
};

struct Tree {
    std::filesystem::path relative;
    std::vector<VersionObject> objects;
};

struct TreeRelation {
    Tree tree;
    std::vector<TreeRelation> children;
};

static void add_object(std::filesystem::path& path, const std::filesystem::path& object_dir, std::vector<VersionObject>& objects) {
    std::string filename = path.filename();
    std::erase_if(objects, [&](const VersionObject& object) {
        return object.name == filename;
    });
    VersionObject object;
    object.name         = filename;
    object.type         = TYPE_BLOB;
    std::string buffer = io::content(path, 0);
    object::create(object_dir, TYPE_BLOB, buffer, object.id);
    objects.push_back(std::move(object));
}

static void rem_object(std::filesystem::path path, const std::filesystem::path& object_dir, std::vector<VersionObject>& objects) {
    std::erase_if(objects, [&](const VersionObject& object) {
        return object.name == path.filename();
    });
}

static std::string tree_builder(TreeRelation& relation, const std::filesystem::path& object_dir) {
    std::string tree_content;
    std::string type;
    std::string id;
    std::string name;

    type = TYPE_TREE;
    for (TreeRelation& child : relation.children) {
        id   = tree_builder(child, object_dir);
        name = child.tree.relative.filename();
        tree_content += type + " " + id + " " + name + "\n";
    }

    type = TYPE_BLOB;
    for (VersionObject& object : relation.tree.objects) {
        id   = object.id;
        name = object.name;
        tree_content += type + " " + id + " " + name + "\n";
    }
    
    object::create(object_dir, TYPE_TREE, tree_content, id);
    return id;
}

static void tree_relation_builder(std::vector<Tree>& trees, TreeRelation& tree_relation, std::filesystem::path current) {
    std::vector<Tree>::iterator iterator = trees.begin(); 
    while (iterator != trees.end()) {
        std::filesystem::path tree_parent = iterator->relative.parent_path();
        if (tree_parent == current) {
            tree_relation.children.push_back({std::move(*iterator), {}});
            iterator = trees.erase(iterator);
            tree_relation_builder(trees, tree_relation.children.back(), tree_relation.children.back().tree.relative);
            iterator = trees.begin();
        }
        else
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

static void tree_from_version(const std::filesystem::path& object_dir, std::vector<Tree>& trees, const std::string& tree_id, std::filesystem::path current) {
    std::vector<std::string> tree_content = io::content_lines(object_dir / tree_id, 1);
    size_t pos;
    Tree tree;
    tree.relative = current;
    for (size_t i = 1; i < tree_content.size(); ++i) {
        std::string line = tree_content[i];
        VersionObject object;
        pos         = line.find(' ');
        object.type = line.substr(0, pos);

        line = line.substr(pos + 1);
        pos  = line.find(' ');

        object.id           = line.substr(0, pos);
        object.name         = line.substr(pos + 1);

        if (object.type == TYPE_TREE)  // tree objects are inserted in tree_builder
            tree_from_version(object_dir, trees, object.id, current / object.name);
        else
            tree.objects.push_back(object);
    }
    trees.push_back(tree);
}

static std::string build(const std::filesystem::path& object_dir, const std::filesystem::path& working_dir, const std::string& tree_id, const std::vector<std::string>& status) {
    std::vector<Tree> trees;
    trees.emplace_back();
    trees.reserve(PREALLOCATE);

    if (!tree_id.empty())
        tree_from_version(object_dir, trees, tree_id, {});

    for (std::string path : status) {
        bool deleted = path.starts_with(PREFIX_DELETED);
        if (deleted)
            path.erase(0, strlen(PREFIX_DELETED));

        std::filesystem::path working_path = working_dir / path;
        std::filesystem::path relative     = path;
        relative = relative.parent_path();

        Tree* tree = nullptr;
        for (Tree& current_tree : trees)
            if (current_tree.relative == relative)
                tree = &current_tree;
        if (tree == nullptr)
            continue;

        if (std::filesystem::is_directory(working_path)) {
            if (deleted)
                std::erase_if(trees, [&](const Tree& tree) {return tree.relative == path;});
            else
                trees.emplace_back(Tree{ .relative = path });
        }

        else if (std::filesystem::is_regular_file(working_path))
            add_object(working_path, object_dir, tree->objects);
            
        else if (deleted)
            rem_object(path, object_dir, tree->objects);
    }

    TreeRelation tree_relation = tree_relation_init(trees);

    return tree_builder(tree_relation, object_dir);
}

LvcError version::create(std::filesystem::path lvc, std::string message, std::string author, std::string inserted_workspace) {
    if (message.empty())
        return VERSION_NO_MESSAGE;
        
    std::filesystem::path prepare_path = lvc / NAME_PREPARE;
    if (!std::filesystem::is_regular_file(prepare_path))
        return NO_FILES_PREPARED;

    std::filesystem::path working_dir = lvc.parent_path();
    std::filesystem::path object_dir  = lvc / NAME_OBJECT;
    std::string workspace_name        = io::content(lvc / NAME_CURRENT, 0);
    std::filesystem::path workspace   = workspace_path(lvc / NAME_WORKSPACE, workspace_name);
    std::string version_id = io::content_first_line(workspace);
    std::vector<std::string> status = version::status(lvc, version_id);
    
    std::string root_id;
    if (!version_id.empty()) {
        std::vector<std::string> version_content = io::content_lines(object_dir / version_id, 1);
        root_id = version_content[VERSION_ROOT_TREE];
    }

    std::string new_root_id = build(object_dir, working_dir, root_id, status);
    
    const std::string nl = "\n";
    std::string buffer = new_root_id + nl + message + nl + author;
    if (!inserted_workspace.empty())
        buffer += nl + inserted_workspace;

    std::string id;
    object::create(object_dir, TYPE_VERSION, buffer, id);
    id += "\n";
    
    RETURN_ERR(io::prefix_file_content(workspace, id.c_str(), id.size()));

    std::error_code error;
    bool deleted = std::filesystem::remove(prepare_path, error);
    if (!deleted || error)
        return PREPARE_RESET_ERROR;

    return SUCCESS;
}