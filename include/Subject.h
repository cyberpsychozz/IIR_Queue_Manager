#pragma once
#include "Teacher.h"
#include "Seminar.h"
#include "Database.h"
#include "Error.h"
#include <vector>
#include <string>

class Database;
class Teacher;

class Subject{
private: 
    int id;
    int teacher_id;
    std::string name;

public:
    // Constructors
    Subject();
    Subject(int subject_id, std::string name, int teacher_id);

    // Getters
    int getId() const;
    int getTeacherId() const;
    const std::string& getName() const;

    // Setters
    void setId(int newId);
    void setTeacherId(int newTeacherId);
    void setName(const std::string& newName);

    // Functions
    FuncResult<Teacher> getTeacher(Database& db) const;
    FuncResult<std::vector<Seminar>> getClasses(Database& db, int group) const;
    FuncError addClass(const Seminar& seminar, Database& db); // TODO 2 функции не дописано
    FuncError deleteClass(const Seminar& seminar, Database& db);
};