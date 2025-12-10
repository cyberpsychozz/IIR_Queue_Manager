#include "Queue.h"
#include "Group.h"
#include <iostream>
#include <TG_Bot.h>
#include <vector>
#include <string>
#include <sstream>

volatile std::sig_atomic_t gSignalStatus = 0;
void signal_handler(int signal) { gSignalStatus = signal; }
//TODO Раскидать группы отдельно
int main() {
    const char* token = std::getenv("BOT_TOKEN");
    if (!token) {
        std::cerr << "ОШИБКА: переменная BOT_TOKEN не установлена!\n";
        return 1;
    }

    TgBot::Bot bot(token);

    auto& db = Database::getInstance("/data/bot.db");
    if (!db.open()) return 1;
    
    // start
    bot.getEvents().onCommand("start", [&bot](TgBot::Message::Ptr message) {
        TgBot::InlineKeyboardMarkup::Ptr keyboard = std::make_shared<TgBot::InlineKeyboardMarkup>();
        
        std::vector<TgBot::InlineKeyboardButton::Ptr> row;
        row.push_back(createBtn("Студент", "role_student"));
        row.push_back(createBtn("Преподаватель", "role_teacher"));
        keyboard->inlineKeyboard.push_back(row);

        bot.getApi().sendMessage(message->chat->id, 
            "Привет! Я бот очередей ИИР.\nВыберите вашу роль:", 
            nullptr, nullptr, keyboard);
    });

    // Обработка нажатий на кнопки
    bot.getEvents().onCallbackQuery([&bot](TgBot::CallbackQuery::Ptr query) {
        std::string data = query->data;
        int64_t chatId = query->message->chat->id;
        int64_t userId = query->from->id; 
    
        // Студент
        if (data == "role_student") {
            std::string tgId = std::to_string(userId);
            auto res = studentByTGID(tgId);

            switch (res.first) {
                // Студент найден
                case (FuncError::OK): 
                    {
                    Student student = res.second.value();
                    auto subjsRes = student.getSubjects();

                    if (subjsRes.first != FuncError::OK) {
                        bot.getApi().answerCallbackQuery(query->id, "Предметы не найдены");
                        return;
                    }

                    // Формируем клавиатуру с предметами
                    std::vector<std::pair<std::string, std::string>> subjButtons;
                    for (const auto& subj : subjsRes.second.value()) {
                        subjButtons.push_back({subj.getName(), "view_" + std::to_string(subj.getId())});
                    }

                    auto keyboard = createKeyboard(subjButtons);
                    
                    std::string text = "Вы авторизованы как: *" + student.getName() + "*\n" +
                                    "Группа: `" + std::to_string(student.getGroupName()) + "`\n\n" +
                                    "Предметы, доступные для вашей группы:";

                    try {
                        bot.getApi().editMessageText(text, chatId, query->message->messageId, "", "Markdown", nullptr, keyboard);
                    } catch (...) {
                        bot.getApi().sendMessage(chatId, text, nullptr, nullptr, keyboard, "Markdown");
                    }
                    
                    bot.getApi().answerCallbackQuery(query->id);
                    return;
                    }
                
                // Студент не зарегистрирован
                case (FuncError::NOT_FOUND):
                    // TODO навести красоту
                    bot.getApi().answerCallbackQuery(query->id, "Вас нет в списке студентов!");
                    bot.getApi().sendMessage(chatId, "Если у вас нет логина, обратитесь к администраторам.");
                    bot.getApi().sendMessage(chatId, "Если у вас есть логин, введите его:");
                    bot.getEvents().onAnyMessage([&bot](TgBot::Message::Ptr message){
                        std::string login = message->text;
                        std::string id = std::to_string(message ->from-> id);
                        int64_t chatId= message->chat->id;
                        auto res = registrate(login, id);
                        switch(res){
                            case(FuncError::OK):{
                                bot.getApi().sendMessage(chatId, "Поздравляю, теперь ты есть в бд и твои данные утекут в даркнет");
                                return;
                            }
                            case(FuncError::NOT_FOUND):{
                                bot.getApi().sendMessage(chatId, "Ты не достоин(можешь попробовать ещё раз, правда все твои попытки тщетны)");
                                return;
                            }
                        }
                    });

                // Ошибки запроса
                default:
                    bot.getApi().answerCallbackQuery(query->id, "Ошибка запроса, попробуйте позже.");
                    return;
            }
        }
        
        // Преподаваель 
        else if (data == "role_teacher") {
            std::string tgId = std::to_string(userId);
            auto res = teacherByTGID(tgId);

            switch (res.first) {
                // Преподаватель найден
                case (FuncError::OK): 
                    {
                    Teacher teacher = res.second.value();
                    auto subjsRes = teacher.getSubjects();

                    if (subjsRes.first != FuncError::OK) {
                        bot.getApi().answerCallbackQuery(query->id, "Предметы не найдены");
                        return;
                    }

                    // Формируем клавиатуру с предметами
                    std::vector<std::pair<std::string, std::string>> subjButtons;
                    for (const auto& subj : subjsRes.second.value()) {
                        subjButtons.push_back({subj.getName(), "view_" + std::to_string(subj.getId())});
                    }

                    auto keyboard = createKeyboard(subjButtons);
                    
                    std::string text = "Вы авторизованы как: *" + teacher.getName() + "*\n\n" +
                                    "Доступные предметы:";

                    try {
                        bot.getApi().editMessageText(text, chatId, query->message->messageId, "", "Markdown", nullptr, keyboard);
                    } catch (...) {
                        bot.getApi().sendMessage(chatId, text, nullptr, nullptr, keyboard, "Markdown");
                    }
                    
                    bot.getApi().answerCallbackQuery(query->id);
                    return;
                    }
                
                // Преподаватель не зарегистрирован
                case (FuncError::NOT_FOUND):
                    // TODO навести красоту
                    bot.getApi().answerCallbackQuery(query->id, "Вас нет в списке преподавателей!");
                    bot.getApi().sendMessage(chatId, "Если у вас нет логина, обратитесь к администраторам.");
                    bot.getApi().sendMessage(chatId, "Если у вас есть логин, введите его:");
                    bot.getEvents().onAnyMessage([&bot](TgBot::Message::Ptr message){
                        std::string login = message->text;
                        std::string id = std::to_string(message ->from-> id);
                        int64_t chatId= message->chat->id;
                        auto res = registrate(login, id);
                        switch(res){
                            case(FuncError::OK):{
                                bot.getApi().sendMessage(chatId, "Вы успешно зарегистрированы");
                                return;
                            }
                            case(FuncError::NOT_FOUND):{
                                bot.getApi().sendMessage(chatId, "Некорректный логин");
                                return;
                            }
                        }
                    });

                // Ошибки запроса
                default:
                    bot.getApi().answerCallbackQuery(query->id, "Ошибка запроса, попробуйте позже.");
                    return;
            }
        }

        // Обработка команд
        std::string action;
        Subject subj;
        subj.setId(0);

        if (StringTools::startsWith(data, "view_")) {
            action = "view";
            subj.setId(std::stoi(data.substr(5)));
        } else if (StringTools::startsWith(data, "join_")) {
            action = "join";
            subj.setId(std::stoi(data.substr(5)));
        } else if (StringTools::startsWith(data, "leave_")) {
            action = "leave";
            subj.setId(std::stoi(data.substr(6)));
        }

        if (subj.getId() > 0) {
            Queue queue(subj.getId());
            subj.sync();
            
            auto qRes = queue.getQueue();
            std::string tgIdStr = std::to_string(userId);

            auto studentRes = studentByTGID(tgIdStr);

            if (studentRes.first == FuncError::OK) {
                int dbStudentId = studentRes.second.value().getId();
                std::string alertText = "";

                if (action == "join") {
                    auto res = queue.push(dbStudentId);
                    if (res.first == FuncError::OK) alertText = "Вы добавлены в очередь!";
                    else alertText = "Ошибка: возможно, вы уже в очереди.";
                } 
                else if (action == "leave") {
                    auto res = queue.give_up(dbStudentId);
                    if (res == FuncError::OK) alertText = "Вы покинули очередь.";
                    else alertText = "Ошибка выхода.";
                }

                int studentPosition = -1;
                bool isInQueue = false;
                
                auto posRes = queue.getPosition(dbStudentId);
                if (posRes.first == FuncError::OK) {
                    studentPosition = posRes.second.value();
                    isInQueue = true;
                }

                std::string response = "Очередь по предмету *" + subj.getName() + "*:\n\n";
                
                if (isInQueue) {
                    response = " *Ваша позиция: " + std::to_string(studentPosition) + "*\n\n" + response;
                }

                if (qRes.first == FuncError::OK && qRes.second.has_value()) {
                    auto list = qRes.second.value();
                    if (list.empty()) response += "Очередь пуста.";
                    else {
                        int count = 1;
                        for (const auto& s : list) {
                            response += std::to_string(count++) + ". " + s.getName();
                            if (s.getId() == dbStudentId) response += " (Вы)";
                            response += "\n";
                        }
                    }
                } else {
                    response += "Ошибка получения списка.";
                }

                auto keyboard = createQueueControls(subj.getId(), isInQueue);
                
                try {
                    bot.getApi().editMessageText(response, chatId, query->message->messageId, "", "Markdown", nullptr, keyboard);
                } catch (...) {}
                
                bot.getApi().answerCallbackQuery(query->id, alertText);
            } 
            else {
                auto teacherRes = teacherByTGID(tgIdStr);
                
                if (teacherRes.first == FuncError::OK) {

                    std::string response = "Очередь студентов по предмету *" + subj.getName() + "*:\n\n";

                    if (qRes.first == FuncError::OK && qRes.second.has_value()) {
                        auto list = qRes.second.value();
                        if (list.empty()) response += "Список пуст.";
                        else {
                            int count = 1;
                            for (const auto& s : list) {
                                response += std::to_string(count++) + ". " + s.getName() + "\n";
                            }
                        }
                    } else {
                        response += "Ошибка получения списка.";
                    }

                    auto keyboard = createTeacherQueueControls(subj.getId());

                    try {
                        bot.getApi().editMessageText(response, chatId, query->message->messageId, "", "Markdown", nullptr, keyboard);
                    } catch (...) {}

                    bot.getApi().answerCallbackQuery(query->id);
                }
                else {
                    bot.getApi().answerCallbackQuery(query->id, "Ошибка доступа. Вы не авторизованы.", true);
                }
            }
        }
    });

    bot.getEvents().onAnyMessage([&bot](TgBot::Message::Ptr message) {
        if (StringTools::startsWith(message->text, "/start")) return;
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
            try {
                longPoll.start();
            } catch (const std::exception& e) {
                std::string error_msg = e.what();
                
                if (error_msg.find("Bad Request") != std::string::npos || 
                    error_msg.find("timed out") != std::string::npos) {
                    
                    std::cerr << "Не критическая ошибка API (Timeout/Bad Request): " << error_msg << std::endl;
                } else {
                    std::cerr << "Общая ошибка (std::exception): " << error_msg << std::endl;
                }
            } catch (...) {
                std::cerr << "Неизвестная ошибка. Перезапуск Long Poll..." << std::endl;
            }
        }
        std::cout << "\nОстановка бота...\n";
    } catch (const std::exception& e) {
        std::cerr << "Критическая ошибка при запуске: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}