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
    Teacher() : Teacher(0, "", "", "") {}

    Teacher(int teacherId, const std::string& teacherName, 
            const std::string& teacherLogin, const std::string& teacherUsernameTg)
        : id(teacherId), name(teacherName), login(teacherLogin), username_tg(teacherUsernameTg) {}

    ~Teacher() = default;

    std::vector<Subject> getSubjects(Database& db) const;

    int getId() const { return id; }
    const std::string& getName() const { return name; }
    const std::string& getLogin() const { return login; }
    const std::string& getUsernameTg() const { return username_tg; }

    void setId(int newId) { id = newId; }
    void setName(const std::string& newName) { name = newName; }
    void setLogin(const std::string& newLogin) { login = newLogin; }
    void setUsernameTg(const std::string& newUsernameTg) { username_tg = newUsernameTg; }
};