#include "Queue.h"
#include "Group.h"
#include <iostream>
#include <TG_Bot.h>
#include <vector>
#include <string>
#include <sstream>

void displayStudentSubjects(TgBot::Bot& bot, int64_t chatId, const Student& student, int32_t messageIdToEdit = 0) {
    auto subjsRes = student.getSubjects();

    if (subjsRes.first != FuncError::OK) {
        try {bot.getApi().sendMessage(chatId, "Предметы для вашей группы не найдены."); }
        catch (const TgBot::TgException& e) {
            std::cerr << "Не удалось отправить сообщение пользователю " << chatId << ": " << e.what() << std::endl;
        }
        return;
    }

    std::vector<std::pair<std::string, std::string>> subjButtons;
    for (const auto& subj : subjsRes.second.value()) {
        subjButtons.push_back({subj.getName(), "view_" + std::to_string(subj.getId())});
    }
    subjButtons.push_back({"<< Меню", "back_to_start"});

    auto keyboard = createKeyboard(subjButtons);
    
    std::string text = "Вы авторизованы как: *" + student.getName() + "*\n" +
                    "Группа: `" + std::to_string(student.getGroupName()) + "`\n\n" +
                    "Предметы, доступные для вашей группы:";

    if (messageIdToEdit != 0) {
        try {
            bot.getApi().editMessageText(text, chatId, messageIdToEdit, "", "Markdown", nullptr, keyboard);
        } catch (...) {
            try {bot.getApi().sendMessage(chatId, text, nullptr, nullptr, keyboard, "Markdown");}
            catch (const TgBot::TgException& e) {
                std::cerr << "Не удалось отправить сообщение пользователю " << chatId << ": " << e.what() << std::endl;
            }
        }
    } else {
        try {bot.getApi().sendMessage(chatId, text, nullptr, nullptr, keyboard, "Markdown");}
        catch (const TgBot::TgException& e) {
            std::cerr << "Не удалось отправить сообщение пользователю " << chatId << ": " << e.what() << std::endl;
        }
    }
}

void displayTeacherSubjects(TgBot::Bot& bot, int64_t chatId, const Teacher& teacher, int32_t messageIdToEdit = 0) {
    auto subjsRes = teacher.getSubjects();

    if (subjsRes.first != FuncError::OK) {
        try {bot.getApi().sendMessage(chatId, "Предметы не найдены.");}
        catch (const TgBot::TgException& e) {
            std::cerr << "Не удалось отправить сообщение пользователю " << chatId << ": " << e.what() << std::endl;
        }
        return;
    }

    std::vector<std::pair<std::string, std::string>> subjButtons;
    for (const auto& subj : subjsRes.second.value()) {
        subjButtons.push_back({subj.getName(), "tview_" + std::to_string(subj.getId())});
    }
    subjButtons.push_back({"<< Меню", "back_to_start"});

    auto keyboard = createKeyboard(subjButtons);
    
    std::string text = "Вы авторизованы как: *" + teacher.getName() + "*\n\n" +
                       "Доступные предметы:";

    if (messageIdToEdit != 0) {
        try {
            bot.getApi().editMessageText(text, chatId, messageIdToEdit, "", "Markdown", nullptr, keyboard);
        } catch (...) {
            try {bot.getApi().sendMessage(chatId, text, nullptr, nullptr, keyboard, "Markdown");}
            catch (const TgBot::TgException& e) {
                std::cerr << "Не удалось отправить сообщение пользователю " << chatId << ": " << e.what() << std::endl;
            }
        }
    } else {
        try {bot.getApi().sendMessage(chatId, text, nullptr, nullptr, keyboard, "Markdown");}
        catch (const TgBot::TgException& e) {
            std::cerr << "Не удалось отправить сообщение пользователю " << chatId << ": " << e.what() << std::endl;
        }
    }
}

void askForLogin(TgBot::Bot &bot, int64_t chatId, int32_t messageIdToDelete, bool ifTeacher = false, const std::string& errorText = "") {
    
    std::string roleText = ifTeacher ? "преподавателей" : "студентов";

    if (messageIdToDelete != 0) {
        try { bot.getApi().deleteMessage(chatId, messageIdToDelete); } catch (...) {}
    }

    if (errorText.empty()) {
        auto forceReply = std::make_shared<TgBot::ForceReply>();
        forceReply->forceReply = true;
        forceReply->selective = true;

        try {bot.getApi().sendMessage(chatId, "Вас нет в списке " + roleText + ".\nДля авторизации введите ваш логин *ответом на это сообщение:*", nullptr, nullptr, forceReply, "Markdown");}
        catch (const TgBot::TgException& e) {
            std::cerr << "Не удалось отправить сообщение пользователю " << chatId << ": " << e.what() << std::endl;
        }
    }
    else {
        TgBot::InlineKeyboardMarkup::Ptr keyboard = std::make_shared<TgBot::InlineKeyboardMarkup>();
    
        std::vector<TgBot::InlineKeyboardButton::Ptr> row;
        
        std::string retryData = ifTeacher ? "retry_teacher" : "retry_student";
        std::string cancelText = ifTeacher ? "Я не преподаватель!" : "Я не студент!";
        
        row.push_back(createBtn("Попробовать снова", retryData));
        keyboard->inlineKeyboard.push_back(row);

        std::vector<TgBot::InlineKeyboardButton::Ptr> row2;
        row2.push_back(createBtn(cancelText, "back_to_start"));
        keyboard->inlineKeyboard.push_back(row2);

        try {bot.getApi().sendMessage(chatId, "Логин не найден в базе " + roleText, nullptr, nullptr, keyboard);} 
        catch (const TgBot::TgException& e) {
            std::cerr << "Не удалось отправить сообщение пользователю " << chatId << ": " << e.what() << std::endl;
        }
    }
}

void setup_handlers(TgBot::Bot &bot) {
    // /start
    bot.getEvents().onCommand("start", [&bot](TgBot::Message::Ptr message) {
        try {
            startMenu(bot, message->chat->id);
        } catch (const std::exception& e) {
            std::cerr << "Ошибка в /start: " << e.what() << std::endl;
        }
    });

    bot.getEvents().onCallbackQuery([&bot](TgBot::CallbackQuery::Ptr query) {
        std::string data = query->data;
        int64_t chatId = query->message->chat->id;
        int64_t userId = query->from->id; 
        std::string tgId = std::to_string(userId);
    
        // Возврат в меню
        if (data == "back_to_start") {
            try { bot.getApi().deleteMessage(chatId, query->message->messageId); } catch (...) {}
            startMenu(bot, chatId);

            try { bot.getApi().answerCallbackQuery(query->id); }
            catch(const std::exception& e) { std::cerr << e.what() << '\n'; }

            return;
        }

        // Повторная авторизация
        if (data == "retry_student") {
            askForLogin(bot, chatId, query->message->messageId);

            try { bot.getApi().answerCallbackQuery(query->id); }
            catch(const std::exception& e) { std::cerr << e.what() << '\n'; }

            return;
        }
        if (data == "retry_teacher") {
            askForLogin(bot, chatId, query->message->messageId, true);

            try { bot.getApi().answerCallbackQuery(query->id); }
            catch(const std::exception& e) { std::cerr << e.what() << '\n'; }

            return;
        }

        if (data == "role_student") {
            auto res = studentByTGID(tgId);
            if (res.first == FuncError::OK) {
                displayStudentSubjects(bot, chatId, res.second.value(), query->message->messageId);
                try { bot.getApi().answerCallbackQuery(query->id); }
                catch(const std::exception& e) { std::cerr << e.what() << '\n'; }
            } else {
                askForLogin(bot, chatId, query->message->messageId); 
                try { bot.getApi().answerCallbackQuery(query->id); }
                catch(const std::exception& e) { std::cerr << e.what() << '\n'; }
            }
            return;
        }
        
        // Преподаватель
        else if (data == "role_teacher") {
            auto res = teacherByTGID(tgId);
            if (res.first == FuncError::OK) {
                displayTeacherSubjects(bot, chatId, res.second.value(), query->message->messageId);
                try { bot.getApi().answerCallbackQuery(query->id); }
                catch(const std::exception& e) { std::cerr << e.what() << '\n'; }
            } else {
                askForLogin(bot, chatId, query->message->messageId, true);
                try { bot.getApi().answerCallbackQuery(query->id); }
                catch(const std::exception& e) { std::cerr << e.what() << '\n'; }
            }
            return;
        }

        // Обработка команд
        std::string action;
        Subject subj;
        subj.setId(0);
        bool isTeacher = false;

        if (StringTools::startsWith(data, "tview_")) { action = "view"; subj.setId(std::stoi(data.substr(6))); isTeacher = true; }
        else if (StringTools::startsWith(data, "view_")) { action = "view"; subj.setId(std::stoi(data.substr(5))); } 
        else if (StringTools::startsWith(data, "join_")) { action = "join"; subj.setId(std::stoi(data.substr(5))); } 
        else if (StringTools::startsWith(data, "leave_")) { action = "leave"; subj.setId(std::stoi(data.substr(6))); } 
        else if (StringTools::startsWith(data, "skip_")){ action = "skip"; subj.setId(std::stoi(data.substr(5))); }

        Queue queue(subj.getId());
        subj.sync();

        if (isTeacher) {
            auto teacherRes = teacherByTGID(tgId);
            if (teacherRes.first == FuncError::OK) {
                auto qRes = queue.getQueue();
                std::string response = "Очередь студентов по предмету *" + subj.getName() + ":*\n\n";
                // response += "Группы: " + subj.getGroups() + "\n\n"

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

                try { bot.getApi().answerCallbackQuery(query->id); }
                catch(const std::exception& e) { std::cerr << e.what() << '\n'; }
            } 
            else {
                try { bot.getApi().answerCallbackQuery(query->id, "Ошибка доступа.", true); }
                catch(const std::exception& e) { std::cerr << e.what() << '\n'; }
            }
        }
        else {
            auto studentRes = studentByTGID(tgId);
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
                    else 
                        alertText = "Ошибка выхода.";
                }
                else if (action == "skip") {
                    auto res = queue.skip(dbStudentId);
                    if (res == FuncError::OK) alertText = "Вы сдвинулись 1 позицию назад.";
                    else 
                        alertText = "Ошибка сдвига.";
                }

                auto qRes = queue.getQueue();

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
                } 
                else 
                    response += "Ошибка получения списка.";

                auto keyboard = createQueueControls(subj.getId(), isInQueue);
                
                try {
                    bot.getApi().editMessageText(response, chatId, query->message->messageId, "", "Markdown", nullptr, keyboard);
                } 
                catch (...) {}
                if (action != "view")
                    try { bot.getApi().answerCallbackQuery(query->id, alertText); }
                    catch(const std::exception& e) { std::cerr << e.what() << '\n'; }
            }
            else {
                try { bot.getApi().answerCallbackQuery(query->id, "Ошибка доступа.", true); }
                catch(const std::exception& e) { std::cerr << e.what() << '\n'; }
            }
        } 
    });

    // Автоудаление сообщений пользователя
    bot.getEvents().onAnyMessage([&bot](TgBot::Message::Ptr message) {

        bool handled = false;

        // Проверяем, является ли сообщение ответом на запрос логина
        if (message->replyToMessage && message->replyToMessage->text.find("логин") != std::string::npos) {

            handled = true;
            std::string login = message->text;
            std::string tgId = std::to_string(message->from->id);
            int64_t chatId = message->chat->id;

            bool isTeacherLogin = message->replyToMessage->text.find("преподавател") != std::string::npos;

            auto res = registrate(login, tgId, isTeacherLogin);

            // Удаляем сообщение пользователя
            try { bot.getApi().deleteMessage(chatId, message->messageId); } catch (...) {}

            if (res == FuncError::OK) {
                // Удаляем запрос бота
                try { bot.getApi().deleteMessage(chatId, message->replyToMessage->messageId); } catch (...) {}
                
                // Показываем меню
                if (isTeacherLogin) {
                    auto tRes = teacherByTGID(tgId);
                    if (tRes.first == FuncError::OK) displayTeacherSubjects(bot, chatId, tRes.second.value());
                } 
                else {
                    auto sRes = studentByTGID(tgId);
                    if (sRes.first == FuncError::OK) displayStudentSubjects(bot, chatId, sRes.second.value());
                }
            } 
            else {
                if (isTeacherLogin)
                    askForLogin(bot, chatId, message->replyToMessage->messageId, true, "Некорректный логин преподавателя");
                else 
                    askForLogin(bot, chatId, message->replyToMessage->messageId, false, "Некорректный логин студента");
            }
        }

        // Если сообщение не было обработано, удаляем его
        if (!handled) {
            try {
                bot.getApi().deleteMessage(message->chat->id, message->messageId);
            } catch (...) {
                // Игнорируем ошибки удаления
            }
        }
    });
}

volatile std::sig_atomic_t gSignalStatus = 0;
void signal_handler(int signal) { gSignalStatus = signal; }

int main() {
    const char* token = std::getenv("BOT_TOKEN");

    TgBot::Bot bot(token);
    auto& db = Database::getInstance("/data/bot.db");
    if (!db.open()) return 1;
    
    setup_handlers(bot);

    std::signal(SIGINT, signal_handler);
    std::signal(SIGTERM, signal_handler);

    try {
        auto me = bot.getApi().getMe();
        std::cout << "Бот запущен: @" << me->username << "\n";
        fflush(stdout);

        TgBot::TgLongPoll longPoll(bot);
        while (gSignalStatus == 0) {
            try {
                longPoll.start();
            } catch (const std::exception& e) {
                std::string error_msg = e.what();
                
                if (error_msg.find("Bad Request") != std::string::npos || 
                    error_msg.find("Forbidden") != std::string::npos ||
                    error_msg.find("timed out") != std::string::npos) {
                    
                    std::cerr << "API Error: " << error_msg << std::endl;
                } else {
                    std::cerr << "System Error: " << error_msg << std::endl;
                }
            } catch (...) {
                std::cerr << "Неизвестная ошибка" << std::endl;
            }
        }
        std::cout << "\nОстановка бота...\n";
    } catch (const std::exception& e) {
        std::cerr << "Критическая ошибка при запуске: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}