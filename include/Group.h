#pragma once
#include "Student.h"

class Group
{
private:
    int group_id;

public:
    // Constructor
    Group(int groupId);

    // Functions
    
    FuncResult<std::vector<Student>> getStudents() const;
    FuncResult<std::vector<Subject>> getSubjects() const;

    FuncError addToSubject(std::optional<std::string> name, std::optional<int> subject_id);
    FuncError deleteFromSubject(std::optional<std::string> name, std::optional<int> subject_id);

    // Leaderboard когда-нибудь когда нам будут платить деньги

};