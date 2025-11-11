#include "Queue.h"
#include "Database.h"
#include "Group.h"
#include "Student.h"


int main() {

    Database db("../data/test.db");
    Queue queue(1, db);

    std::vector<Student> students = queue.getAllStudents(queue);

    for (int i = 0; i < students.size(); ++i) {
        std::cout << "Pos " << i + 1 << ": " << students[i].getName() << std::endl;
    }
    return 0;
}
// вместо классов компоненты 