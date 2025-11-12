#pragma once
#include "Teacher.h"
#include "Seminar.h"
#include "Database.h"
#include "Error.h"
#include <vector>
#include <string>

class Subject{
private: 
    int id;
    std::string name;
    int teacher_id;

public:

    Subject(int subject_id, std::string name, int teacher_id)
        : id(subject_id), name(name), teacher_id(teacher_id) {}

    FuncResult<Teacher> getTeacher(Database& db) const;
    FuncResult<std::vector<Seminar>> getClasses(Database& db, int group) const;
    FuncError addClass(const Seminar& seminar, Database& db);
    FuncError deleteClass(const Seminar& seminar, Database& db);
};