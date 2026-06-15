#pragma once
#include <sqlite3.h>

class DataBase {
    sqlite3* sql = nullptr;
    void open_database();
    void exec(const char* query);
public:
    void initialize_existing();
    void initialize_new();
    ~DataBase();
};

extern DataBase db;

inline constexpr const char* initialization_schema = R"sql(
PRAGMA foreign_keys = ON;

CREATE TABLE branch (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    name TEXT NOT NULL UNIQUE,
    parent_branch_id INTEGER,
    parent_version_id INTEGER,
    is_merged INTEGER NOT NULL DEFAULT 0,

    FOREIGN KEY(parent_branch_id) REFERENCES branch(id),
    FOREIGN KEY(parent_version_id) REFERENCES version(id)
);

CREATE TABLE version (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    branch_id INTEGER NOT NULL,
    description TEXT NOT NULL,
    created_at TEXT NOT NULL DEFAULT CURRENT_TIMESTAMP,
    parent_version_id INTEGER,
    merge_parent_version_id INTEGER,

    FOREIGN KEY(branch_id) REFERENCES branch(id),
    FOREIGN KEY(parent_version_id) REFERENCES version(id),
    FOREIGN KEY(merge_parent_version_id) REFERENCES version(id)
);

CREATE TABLE blob (
    hash TEXT PRIMARY KEY,
    blob_path TEXT NOT NULL,
    created_at_version_id INTEGER NOT NULL,
    size_original INTEGER NOT NULL,
    size_compressed INTEGER NOT NULL,
    is_local INTEGER NOT NULL DEFAULT 1,
    url TEXT,

    FOREIGN KEY(created_at_version_id) REFERENCES version(id)
);

CREATE TABLE version_blob (
    version_id INTEGER NOT NULL,
    path TEXT NOT NULL,
    blob_hash TEXT NOT NULL,

    PRIMARY KEY(version_id, path),
    FOREIGN KEY(version_id) REFERENCES version(id),
    FOREIGN KEY(blob_hash) REFERENCES blob(hash)
);

CREATE TABLE state (
    branch_id INTEGER NOT NULL,
    FOREIGN KEY(branch_id) REFERENCES branch(id)
);

CREATE INDEX idx_version_branch
ON version(branch_id);

CREATE INDEX idx_version_blob_hash
ON version(blob_hash);
)sql";

