// Queue.h
#pragma once
#include "Student.h"
#include "Database.h"
#include <expected>
#include <vector>

enum class QueueError; // forward declaration

class Queue {
private:
    int _Subject_id;
    Database& db;

public:
    Queue(int subject_id, Database& database)
        : _Subject_id(subject_id), db(database) {}

    // === Операции с возвратом expected ===
    std::expected<void, QueueError> push(int student_id);
    std::expected<void, QueueError> pop();
    std::expected<void, QueueError> swap(int pos1, int pos2);
    std::expected<void, QueueError> skip();
    std::expected<void, QueueError> give_up(int student_id);

    std::expected<int, QueueError> getPosition(int student_id) const;
    std::expected<int, QueueError> getLen() const;

    std::expected<std::vector<Student>, QueueError> getAllStudents() const;
};