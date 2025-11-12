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
    Database(const std::string& path) : _path(path){
        if(!open()){
            std::cerr << "can't open a database, try again with a correct path";
        }
    }

    ~Database(){
        close();
    };

    sqlite3* get_conn() const {return _conn;}
    std::string get_path() const {return _path;}

    bool open(){
        return sqlite3_open(_path.c_str(), &_conn) == SQLITE_OK;
    }

    bool close(){
        return sqlite3_close(_conn) == SQLITE_OK;
    }

    get_teacher_subjects;

    get_student subjects;

};