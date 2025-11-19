#pragma once
#include <string>
#include <vector>
#include "Subject.h"
#include "Error.h"

class Subject;

class Teacher {
private:
    int id;
    std::string name;
    std::string login;
    std::string username_tg;

public:
    // Constructors
    Teacher();
    Teacher(int teacherId, const std::string& teacherName, 
            const std::string& teacherLogin, const std::string& teacherUsernameTg);
    ~Teacher();

    // Getters
    int getId() const;
    const std::string& getName() const;
    const std::string& getLogin() const;
    const std::string& getUsernameTg() const;

    // Setters
    void setId(int newId);
    void setName(const std::string& newName);
    void setLogin(const std::string& newLogin);
    void setUsernameTg(const std::string& newUsernameTg);

    // Functions
    FuncResult<std::vector<Subject>> getSubjects() const;
};