#include <Student.h>
#include <tgbot/tgbot.h>

void startMenu(TgBot::Bot& bot, int64_t chatId);

FuncResult <Student> studentByTGID (const std::string& TG_id);
FuncResult <Teacher> teacherByTGID (const std::string& TG_id);
FuncError registrate(const std::string login, const std::string TG_Id, bool prepod=0);

TgBot::InlineKeyboardButton::Ptr createBtn(const std::string& text, const std::string& callbackData);

TgBot::InlineKeyboardMarkup::Ptr createKeyboard(const std::vector<std::pair<std::string, std::string>>& buttonsData, bool asRows = true);

TgBot::InlineKeyboardMarkup::Ptr createQueueControls(int subjectId, bool isInQueue);

TgBot::InlineKeyboardMarkup::Ptr createTeacherQueueControls(int subjectId);