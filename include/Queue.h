// Queue.h
#pragma once
#include "Student.h"
#include "Database.h"
#include "QueueError.h"
#include <vector>

class Queue {
private:
    int _Subject_id;
    Database& db;

public:
    Queue(int subject_id, Database& database)
        : _Subject_id(subject_id), db(database) {}

    // === Методы с ошибками ===
    QueueResult<int> push(int student_id);
    QueueResult<bool> pop();
    QueueResult<bool> swap(int pos1, int pos2);
    QueueResult<bool> skip();
    QueueResult<bool> give_up(int student_id);

    QueueResult<int> getPosition(int student_id) const;
    QueueResult<int> getLen() const;

    QueueResult<std::vector<Student>> getAllStudents() const;
};