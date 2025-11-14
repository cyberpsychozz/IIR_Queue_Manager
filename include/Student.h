#pragma once
#include <string>
#include "Subject.h"
#include "Error.h"
#include "Database.h"

class Subject;
class Database;
class Student 
{
private:
    int id;
    int group_name;
    std::string Name;
    std::string login;
    std::string username_tg;     // Telegram username
    // Arrangement
    // Score
public:
    // Constructor
    Student();

    // Getters
    int getId() const;
    int getGroupName() const;
    const std::string& getName() const;
    const std::string& getLogin() const;
    const std::string& getUsernameTg() const;

    // Setters
    void setId(int newId);
    void setGroupName(int newGroupName);
    void setName(const std::string& newName);
    void setLogin(const std::string& newLogin);
    void setUsernameTg(const std::string& newUsernameTg);

    // Functions
    FuncResult<std::vector<Subject>> getSubjects() const;

};