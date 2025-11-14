#pragma once
#include "Student.h"
#include "Subject.h"
#include "Database.h"
#include <vector>

class Group // TODO Реализовать функции класса
{
private:
    int group_id;

public:
    // Constructor
    Group(int groupId);

    // Functions
    FuncResult<std::vector<Student>> getStudents() const;
    FuncResult<std::vector<Subject>> getSubjects() const;

    void addStudent(int student_id);
    void deleteStudent(int student_id);

    void addSubject(int student_id);
    void deleteSubject(int student_id);

    // Leaderboard когда-нибудь когда нам будут платить деньги

};