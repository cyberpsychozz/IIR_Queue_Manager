#pragma once
#include "Teacher.h"
#include "Seminar.h"
#include "Database.h"
#include <vector>
#include <string>

class Subject{
private: 
    int id;
    std::string name;
    int teacher_id;

    Teacher getTeacher(Database& db) const;
    std::vector<Seminar> getClasses(Database& db) const;
    void addClass(const Seminar& seminar, Database& db);
    void deleteClass(const Seminar& seminar, Database& db);
};