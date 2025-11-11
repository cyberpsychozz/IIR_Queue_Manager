#pragma once
#include <string>

class Student 
{
private:
    int id;
    std::string Name;
    std::string login;
    std::string username_tg;     // Telegram username
    std::string group_name;
    // Arrangement
    // Score
public:
    // Getters
    int getId() const { return id; }
    const std::string& getName() const { return Name; }
    const std::string& getLogin() const { return login; }
    const std::string& getUsernameTg() const { return username_tg; }
    const std::string& getGroupName() const { return group_name; }

    // Setters
    void setId(int newId) { id = newId; }
    void setName(const std::string& newName) { Name = newName; }
    void setLogin(const std::string& newLogin) { login = newLogin; }
    void setUsernameTg(const std::string& newUsernameTg) { username_tg = newUsernameTg; }
    void setGroupName(const std::string& newGroupName) { group_name = newGroupName; }

};