#pragma once
#include "Student.h"
#include "Database.h"
#include "Error.h"
#include <vector>

class Queue {
private:
    int _Subject_id;
    Database& db;

public:
    Queue(int subject_id, Database& database)
        : _Subject_id(subject_id), db(database) {}

    // === Методы с ошибками ===
    FuncResult<int> push(int student_id);
    FuncResult<bool> pop();
    FuncResult<bool> swap(int pos1, int pos2);
    FuncResult<bool> skip();
    FuncResult<bool> give_up(int student_id);

    FuncResult<int> getPosition(int student_id) const;
    FuncResult<int> getLen() const;

    FuncResult<std::vector<Student>> getAllStudents() const;
};