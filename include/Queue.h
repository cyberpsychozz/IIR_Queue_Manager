#pragma once
#include "Student.h"

class Queue {
private:
    int _Subject_id;
    

public:
    // Constructor
    Queue(int subject_id);

    // Functions
    FuncResult<int> push(int student_id);
    FuncResult<bool> pop();
    FuncResult<bool> swap(int pos1, int pos2);
    FuncResult<bool> skip();
    FuncResult<bool> give_up(int student_id);

    FuncResult<int> getPosition(int student_id) const;
    FuncResult<int> getLen() const;

    FuncResult<std::vector<Student>> getQueue() const;
};