#include "Queue.h"
#include "Group.h"
#include <iostream>
#include <TG_Bot.h>
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

    auto& db = Database::getInstance("/app/data/test.db");
    if (!db.open()) return 1;
    

    bot.getEvents().onCommand("start", [&bot](TgBot::Message::Ptr message) {
        bot.getApi().sendMessage(message->chat->id, 
            "Привет! Я бот очередей ИИР.\n\n"
            "Кто вы?\n"
            "/student - Студент\n"
            "/teacher - Преподаватель\n");
    });

    bot.getEvents().onCommand("student", [&bot](TgBot::Message::Ptr message) {
        std::string tgId = std::to_string(message->from->id);
        auto res = studentByTGID(tgId);

        if (res.first != FuncError::OK) {
            bot.getApi().sendMessage(message->chat->id, "Вас нет в списке студентов. Обратитесь к администратору.");
            return;
        }

        Student student = res.second.value();
        
        std::string response = "Вы авторизованы как: " + student.getName() + "\n";
        response += "Ваша группа: " + std::to_string(student.getGroupName()) + "\n\n";

        auto subjsRes = getSubjects(student.getGroupName());

        if (subjsRes.first != FuncError::OK) {
            bot.getApi().sendMessage(message->chat->id, response + "Предметы не найдены.");
            return;
        }

        response += "Выберите предмет:\n";
        
        for (const auto& subj : subjsRes.second.value()) {
            response += "/q_" + std::to_string(subj.getId()) + " — " + subj.getName() + "\n";
        }

        bot.getApi().sendMessage(message->chat->id, response);
    });


    bot.getEvents().onAnyMessage([&bot](TgBot::Message::Ptr message) {
        if (StringTools::startsWith(message->text, "/start") || 
            StringTools::startsWith(message->text, "/student")) {
            return;
        }

        if (StringTools::startsWith(message->text, "/q_")) {
            try {
                std::string idStr = message->text.substr(3, 1);
                int subjectId = std::stoi(idStr);

                Queue queue(subjectId);
                
                auto qRes = queue.getQueue();

                std::string response = "Очередь по предмету (ID " + idStr + "):\n";
                
                if (qRes.first == FuncError::OK && qRes.second.has_value()) {
                    auto list = qRes.second.value();
                    if (list.empty()) {
                        response += "Очередь пуста.";
                    } else {
                        int count = 1;
                        for (const auto& s : list) {
                            response += std::to_string(count++) + ". " + s.getName() + "\n";
                        }
                    }
                } else {
                    response += "Ошибка получения списка очереди.";
                }

                bot.getApi().sendMessage(message->chat->id, response);

            } catch (const std::exception& e) {
                bot.getApi().sendMessage(message->chat->id, "Некорректный номер предмета.");
            }
        }
    });

    std::signal(SIGINT, signal_handler);
    std::signal(SIGTERM, signal_handler);

    try {
        auto me = bot.getApi().getMe();
        std::cout << "Бот запущен: @" << me->username << " (" << me->firstName << ")\n";
        std::cout << "Нажми Ctrl+C для остановки\n";
        fflush(stdout);

        TgBot::TgLongPoll longPoll(bot);
        while (gSignalStatus == 0) {
            longPoll.start();
        }
        std::cout << "\nОстановка бота...\n";
    } catch (const std::exception& e) {
        std::cerr << "Ошибка: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}