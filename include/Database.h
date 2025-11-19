#pragma once
#include <sqlite3.h>
#include <string>
#include <memory>

class Database {
private:
    sqlite3* conn = nullptr;
    std::string path;

    // Приватный конструктор — нельзя создать напрямую
    Database(const std::string& db_path);
    
    // Запрещаем копирование
    Database(const Database&) = delete;
    Database& operator=(const Database&) = delete;

public:
    ~Database();

    // Единственный способ получить экземпляр
    static Database& getInstance(const std::string& db_path = "bot.db");

    bool open();
    void close();
    sqlite3* get_conn() const { return conn; }
};