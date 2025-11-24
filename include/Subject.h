#pragma once
#include "Teacher.h"
#include "Seminar.h"
#include "Database.h"
#include "Error.h"
#include <vector>

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
    
    FuncResult<Teacher> getTeacher() const;
    FuncResult<std::vector<Seminar>> getClasses(int group) const;
    FuncError addClass(const Seminar& seminar, int group);
    FuncError deleteClass(int sem_id);

    FuncResult<int> addSubject(std::string groups);
    FuncError deleteSubject();
};