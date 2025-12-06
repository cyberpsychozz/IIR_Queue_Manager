#include <Student.h>
#include <tgbot/tgbot.h>

FuncResult <Student> studentByTGID (const std::string& TG_id);

FuncResult <std::vector<Subject>> getSubjects(int group_id);

TgBot::InlineKeyboardButton::Ptr createBtn(const std::string& text, const std::string& callbackData);

TgBot::InlineKeyboardMarkup::Ptr createKeyboard(const std::vector<std::pair<std::string, std::string>>& buttonsData, bool asRows = true);

TgBot::InlineKeyboardMarkup::Ptr createQueueControls(int subjectId);