#pragma once
#include <sqlite3.h>
#include <string>
#include <vector>
#include <iostream>
#include "Student.h"
#include "Queue.h"

class Database {
private:
    sqlite3* _conn;
    std::string _path;

public:
    Database(const std::string& path);
    ~Database();

    sqlite3* get_conn() const;
    std::string get_path() const;

    bool open();
    bool close();
};