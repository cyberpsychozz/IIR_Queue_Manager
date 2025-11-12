#include "Queue.h"
#include "Database.h"
#include "Group.h"
#include "Student.h"
#include "Subject.h"


int main() {

    Database db("../data/test.db");
    Subject subj(1, "ООП", 1);

    FuncResult<Teacher> res = subj.getTeacher(db);
    if (res.second) {
        std::cout << (*res.second).getName() << std::endl;
    }
    else {
        printf("%s\n", res.first);
    }

    // FuncResult<std::vector<Seminar>> res2 = subj.getClasses(db, 24940);

    // if (res2.second) {
    //     for (auto v : (*res2.second)) {
    //         std::cout << v.id << v.date << v.comment <<std::endl;
    //     }
    // }
    // else {
    //     printf("%s\n", res2.first);
    // }


    
}