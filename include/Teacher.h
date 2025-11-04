#pragma once
#include <string>
#include <vector>
#include "Database.h"
#include "Subject.h"

class Teacher {
private:
    int id;
    std::string name;
    std::string login;
    std::string username_tg;

public:
    // Constructor
    
    std::vector<Subject> getSubjects(Database& db) const;
};