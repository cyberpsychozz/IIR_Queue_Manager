#pragma once
#include "Student.h"
#include "Database.h"
#include "Error.h"
#include <vector>

class Database;
struct Student;
class Queue {
private:
    int _Subject_id;
    Database& db;

public:
    // Constructor
    Queue(int subject_id, Database& database);

    // === Методы с ошибками ===
    FuncResult<int> push(int student_id); // FIXME чё-то как-то не так работает, надо тестить (все функции)
    FuncResult<bool> pop();
    FuncResult<bool> swap(int pos1, int pos2);
    FuncResult<bool> skip();
    FuncResult<bool> give_up(int student_id);

    FuncResult<int> getPosition(int student_id) const;
    FuncResult<int> getLen() const;

    FuncResult<std::vector<Student>> getQueue() const; // Переименовал
};