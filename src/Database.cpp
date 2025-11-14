// Database.cpp
#include "Database.h"
#include <iostream>

Database::Database(const std::string& db_path) : path(db_path) {}

Database::~Database() {
    close();
}

Database& Database::getInstance(const std::string& db_path) {
    // Статический локальный объект — создаётся один раз
    static Database instance(db_path);
    return instance;
}

bool Database::open() {
    if (conn) return true; // Уже открыта

    int rc = sqlite3_open(path.c_str(), &conn);
    if (rc != SQLITE_OK) {
        std::cerr << "Can't open database: " << sqlite3_errmsg(conn) << std::endl;
        sqlite3_close(conn);
        conn = nullptr;
        return false;
    }
    return true;
}

void Database::close() {
    if (conn) {
        sqlite3_close(conn);
        conn = nullptr;
    }
}