#pragma once
#include "Student.h"
#include "Subject.h"
#include "Database.h"
#include <vector>

class Group
{
private:
    int group_id;

public:
    std::vector<Student> getStudents(Database& db) const;
    std::vector<Subject> getSubjects(Database& db) const;

    void addStudent(int student_id, Database& db);
    void deleteStudent(int student_id, Database& db);

    void addSubject(int student_id, Database& db);
    void deleteSubject(int student_id, Database& db);

    // Leaderboard когда-нибудь когда нам будут платить деньги

};