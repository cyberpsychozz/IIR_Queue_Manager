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
        std::cout << (*res.second).getName();
    }
    else {
        printf("%s\n", res.first);
    }
    // subj.getClasses(db, 24940);
    


    
}