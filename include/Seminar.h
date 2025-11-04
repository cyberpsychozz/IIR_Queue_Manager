#pragma once
#include <ctime>
#include <string>

struct Seminar
{
    int id;
    time_t date; // Дата и время семинара
    std::string comment; // Комментарий от преподавателя 
                        // (Например, об отмене семинара или количестве студентов в очереди в этот день)
};