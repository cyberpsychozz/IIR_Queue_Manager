#include "Database.h"

Database::Database(const std::string& path) : _path(path){
        if(!open()){
            std::cerr << "can't open a database, try again with a correct path";
        }
    }

Database::~Database(){
    close();
};

sqlite3* Database::get_conn() const {return _conn;}
std::string Database::get_path() const {return _path;}

bool Database::open(){
    return sqlite3_open(_path.c_str(), &_conn) == SQLITE_OK;
}

bool Database::close(){
    return sqlite3_close(_conn) == SQLITE_OK;
}