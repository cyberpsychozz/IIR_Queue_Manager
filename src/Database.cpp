#include "Database.h"
#include <iostream>

Database::Database(const std::string& db_path) : path(db_path) {}

Database::~Database() {
    close();
}

/*
Возвращает единственный существующий экземпляр singleton-объекта database
*/
Database& Database::getInstance(const std::string& db_path) {
    // Статический локальный объект — создаётся один раз
    static Database instance(db_path);
    return instance;
}

/*
Открывает базу данных и позволяет с ней работать
Используется в паре с database.close()
*/
bool Database::open() {
    if (conn) return true; // Уже открыта

    int rc = sqlite3_open(path.c_str(), &conn);
    if (rc != SQLITE_OK) {
        std::cerr << "Can't open database: " << sqlite3_errmsg(conn) << std::endl;
        sqlite3_close(conn);
        conn = nullptr;
        return false;
    }
    sqlite3_exec(conn, "PRAGMA journal_mode=WAL;", nullptr, nullptr, nullptr);
    sqlite3_exec(conn, "PRAGMA synchronous=NORMAL;", nullptr, nullptr, nullptr);
    return true;
}

// Закрывает базу данных
void Database::close() {
    if (conn) {
        sqlite3_close(conn);
        conn = nullptr;
    }
}