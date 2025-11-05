#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <ctime>
#include <map>

using namespace std;

class Queue{
private:
    vector<Student&> queue;
public: 
    int push();
    int pop();
    int swap(); //можно потом сделать по согласию двух челиков между собой
    int skip(); // пропуск через 1
    int give_up(); // удаление из очереди по желанию ученика
    // Constructors
};

class Class{
private:
    // Queue
    Queue queue;
    
    // Comment
    string comment;
public:
    int show_queue();
};

class Subject{
private: 
    // Teachers
    Teacher _prepod;
    // Classes
    map<time_t, Class> _classes;
// Nikita_lox
// Ilya_lox
public:
    explicit Subject(Teacher prepod, map<time_t, Class> classes) : _prepod(prepod), _classes(classes){}

    Teacher prepod(void) const {return _prepod;}
    map<time_t, Class> classes(void) const {return _classes;}


};

class Group
{
private:
    // Subjects
    vector<Subject> subjects;
    // Students
    vector<Student> students;
    // Leaderboard когда-нибудь когда нам будут платить деньги
public:


};

class Student 
{
private:
    // Name
    string Name;
    // Login
    string login;
    // TG username
    string username_tg;
    // Arrangement
    // Score
public:
    
};

class Teacher{
private:
    // Name
    // Login
    // TG username
    // Subjects
};