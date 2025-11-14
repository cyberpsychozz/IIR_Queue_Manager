#include "Queue.h"
#include "Database.h"
#include "Group.h"
#include "Student.h"
#include "Subject.h"
#include <vector>


int main() {

    auto& db = Database::getInstance("../data/test.db");
    Subject subj(1, "ООП", 1);
    Queue queue(1, db);
 
    // FuncResult<Teacher> res = subj.getTeacher(db);
    // if (res.second) {
    //     std::cout << (*res.second).getName() << std::endl;
    // }
    // else {
    //     printf("%s\n", res.first);
    // }


    // FuncResult<std::vector<Seminar>> res2 = subj.getClasses(db, 24940);

    // if (res2.second) {
    //     for (auto v : (*res2.second)) {
    //         std::cout << v.id << "   " << v.date << "   " << v.comment <<std::endl;
    //     }
    // }
    // else {
    //     // printf("%s\n", res2.first);
    //     // Напечатать ошибку
    // }

    // FuncResult<std::vector<Student>> q= queue.getQueue();
    // for (auto v : (*q.second)) {
    //         std::cout << v.getName() << v.getGroupName() <<std::endl;
    // }

    // std::cout << "\npush\n"<<std::endl;
    // queue.push(1);
    // queue.push(2);
    // queue.push(3);
    // queue.push(4);
    // queue.push(5);

    // q = queue.getQueue();

    // for (auto v : (*q.second)) {
    //         std::cout << v.getName() << v.getGroupName() <<std::endl;
    // }

    // std::cout << "\npop\n"<<std::endl;
    
    // queue.pop();

    auto res = queue.swap(1, 3);
    if (res.first != FuncError::OK) {
        std::cerr << "Swap failed: " << static_cast<int>(res.first) << std::endl;
    } else {
        std::cout << "Swap OK\n";
    }

    


    // std::cout << "\nskip\n"<<std::endl;
    
    // queue.skip();

    // q = queue.getQueue();

    // for (auto v : (*q.second)) {
    //         std::cout << v.getName() << v.getGroupName() <<std::endl;
    // }

    // std::cout << "\ngive up\n"<<std::endl;
    
    // queue.give_up(3);

    // q = queue.getQueue();

    // for (auto v : (*q.second)) {
    //         std::cout << v.getName() << v.getGroupName() <<std::endl;
    // }

    // queue.pop();
    // queue.pop();
    // queue.pop();
    
    // TODO Тесты всех функций
}