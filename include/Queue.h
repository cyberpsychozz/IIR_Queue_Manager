#pragma once
#include "Student.h"
#include "Database.h"
#include <vector>

class Queue{
private:
    int subject_id;  // Предмет очереди
    Database& db;    // Ссылка на очередь

public: 
    // Constructors

    int push(int student_id);
    int pop();
    int swap(int pos1, int pos2); // Обмен позициями в очереди (служебный или по согласию)
    int skip(); // Пропуск одного человека вперёд
    int give_up(int student_id); // Удаление из очереди по желанию Студента

    int getPosition(int student_id) const; // Своя позиция в очереди
    std::vector<int> getQueue() const;  // Вся очередь
    int getLen() const;  // Длина очереди
};