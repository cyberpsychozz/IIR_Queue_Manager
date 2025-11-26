#include "Queue.h"
#include "Database.h"
#include "Group.h"
#include "Student.h"
#include "Subject.h"
#include <vector>
#include <iostream>
#include <tgbot/tgbot.h>

volatile std::sig_atomic_t gSignalStatus = 0;
void signal_handler(int signal) { gSignalStatus = signal; }

int main() {
        const char* token = std::getenv("BOT_TOKEN");
        if (!token) {
                std::cerr << "ОШИБКА: переменная BOT_TOKEN не установлена!\n";
                return 1;
        }

        TgBot::Bot bot(token);

        auto& db = Database::getInstance("../data/test.db");
        db.open();

        Queue OOP(1);
        Queue PAC(2);


        bot.getEvents().onCommand("start", [&bot](TgBot::Message::Ptr message) {
                bot.getApi().sendMessage(message->chat->id, "Привет! Я бот\n\n"
                "Кто вы?\n"
                "/student - Студент\n"
                "/teacher - Преподаватель\n");
        });

        bot.getEvents().onCommand("join", [&bot](TgBot::Message::Ptr msg) {
                auto userId = msg->from->id;
                std::string name = msg->from->firstName;
                if (!msg->from->lastName.empty()) name += " " + msg->from->lastName;
                
                bot.getApi().sendMessage(msg->chat->id, userId + " " + name);
        });

        bot.getEvents().onAnyMessage([&bot](TgBot::Message::Ptr message) {
                printf("User wrote %s\n", message->text.c_str());
                if (StringTools::startsWith(message->text, "/start")) {
                return;
                }
                bot.getApi().sendMessage(message->chat->id, "Your message is: " + message->text);
        });
        
        std::signal(SIGINT, signal_handler);
        std::signal(SIGTERM, signal_handler);
        
        try {
                auto me = bot.getApi().getMe();
                std::cout << "Бот запущен: @" << me->username << " (" << me->firstName << ")\n";
                std::cout << "Нажми Ctrl+C для остановки\n";

                TgBot::TgLongPoll longPoll(bot);
                while (gSignalStatus == 0) {
                longPoll.start();
                }
                std::cout << "\nОстановка бота...\n";
        } catch (const std::exception& e) {
                std::cerr << "Ошибка: " << e.what() << std::endl;
                return 1;
        }
        

//     std::cout << "penis" << std::endl;
    
//     std::cout << "penis2" << std::endl;
//     Subject subj(1, "ООП", 1);
//     Queue queue(1);
 
//     // FuncResult<Teacher> res = subj.getaTeacher(db);
//     // if (res.second) {
//     //     std::cout << (*res.second).getName() << std::endl;
//     // }
//     // else {
//     //     printf("%s\n", res.first);
//     // }


//     // FuncResult<std::vector<Seminar>> res2 = subj.getClasses(db, 24940);

//     // if (res2.second) {
//     //     for (auto v : (*res2.second)) {
//     //         std::cout << v.id << "   " << v.date << "   " << v.comment <<std::endl;
//     //     }
//     // }
//     // else {
//     //     // printf("%s\n", res2.first);
//     //     // Напечатать ошибку
//     // }

//     FuncResult<std::vector<Student>> q= queue.getQueue();
//     for (auto v : (*q.second)) {
//             std::cout << v.getId() << " " << v.getName() << " " << v.getGroupName() <<std::endl;
//     }

//     std::cout << "\npush\n"<<std::endl;
//     queue.push(1);
//     queue.push(2);
//     queue.push(3);
//     queue.push(4);
    
//     q= queue.getQueue();
//     for (auto v : (*q.second)) {
//             std::cout << v.getId() << " " << v.getName() << " " << v.getGroupName() <<std::endl;
//     }

//     std::cout << "\n penis\n" << std::endl; 
//     queue.pop();
//     std::cout << "\n penis\n" << std::endl;

//     q = queue.getQueue();
//     for (auto v : (*q.second)) {
//             std::cout << v.getId() << " " << v.getName() << " " << v.getGroupName() <<std::endl;
//     }

//     // q = queue.getQueue();

//     // for (auto v : (*q.second)) {
//     //         std::cout << v.getName() << v.getGroupName() <<std::endl;
//     // }

//     // std::cout << "\npop\n"<<std::endl;
    
//     // queue.pop();

//     // auto res = queue.swap(1, 3);
//     // if (res.first != FuncError::OK) {
//     //     std::cerr << "Swap failed: " << static_cast<int>(res.first) << std::endl;
//     // } else {
//     //     std::cout << "Swap OK\n";
//     // }

    


//     // std::cout << "\nskip\n"<<std::endl;
    
//     // queue.skip();

//     // q = queue.getQueue();

//     // for (auto v : (*q.second)) {
//     //         std::cout << v.getName() << v.getGroupName() <<std::endl;
//     // }

//     // std::cout << "\ngive up\n"<<std::endl;
    
//     // queue.give_up(3);

//     // q = queue.getQueue();

//     // for (auto v : (*q.second)) {
//     //         std::cout << v.getName() << v.getGroupName() <<std::endl;
//     // }

//     // queue.pop();
//     // queue.pop();
//     // queue.pop();
    
//     // TODO Тесты всех функций
}