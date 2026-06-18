#include <database.hpp>
#include <state.hpp>
#include <iostream>
#include <stdexcept>

void DataBase::open() {
    std::string directory = state.lvc_directory + "/lvc.db";
    if (sqlite3_open_v2(directory.c_str(), &sql, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, nullptr))
        throw std::runtime_error(sqlite3_errmsg(sql));
    exec("PRAGMA foreign_keys = ON;");
}

void DataBase::exec(const char* query) {
    char* error = nullptr;
    if (sqlite3_exec(sql, query, nullptr, nullptr, &error)) {
        std::cout << "\033[31m" << "sqlite error: " << "\033[0m" << (error ? error : sqlite3_errmsg(sql)) << "\n";
        sqlite3_free(error);
        throw std::runtime_error("Couldn't query sqlite.");
    }
}

void DataBase::create_new() {
    open();
    exec("BEGIN;");
    exec(initialization_schema);
    exec("COMMIT;");
}

DataBase::~DataBase() {
    sqlite3_close(sql);
}