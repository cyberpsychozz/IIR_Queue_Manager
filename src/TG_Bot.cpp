#include <TG_Bot.h>

// Возвращает студента по его TG Id
// Если студент не найден возвращает FuncError::NOT_FOUND
FuncResult <Student> studentByTGID (const std::string& TG_id) {
    auto &db = Database::getInstance();

    if (!db.get_conn()) {
        return {FuncError::DB_NOT_OPEN, std::nullopt};
    }

    const char* sql = R"(
        SELECT Id, Groups, Name, Login FROM Students
        WHERE TG_id = ?
    )";

    sqlite3_stmt* stmt = nullptr;
    auto rc = sqlite3_prepare_v2(db.get_conn(), sql, -1, &stmt, nullptr);

    sqlite3_bind_text(stmt, 1, TG_id.c_str(), -1, SQLITE_STATIC);

    Student student;

    rc = sqlite3_step(stmt);
    
    if (rc == SQLITE_ROW) {
        int studentId = sqlite3_column_int(stmt, 0);
        int group = sqlite3_column_int(stmt, 1);
        const unsigned char* name = sqlite3_column_text(stmt, 2);
        const unsigned char* login = sqlite3_column_text(stmt, 3);
        
        student.setId(studentId);
        student.setGroupName(group);
        student.setName(std::string(reinterpret_cast<const char*>(name)));
        student.setLogin(std::string(reinterpret_cast<const char*>(login)));
        student.setUsernameTg(TG_id);
    } 

    else if (rc == SQLITE_DONE) {
        sqlite3_finalize(stmt);
        return {FuncError::NOT_FOUND, std::nullopt};
    }

    else {
        sqlite3_finalize(stmt);
        return {FuncError::STEP_FAILED, std::nullopt};
    }
    
    sqlite3_finalize(stmt);

    return {FuncError::OK, student};
}

// Возвращает преподавателя по его TG Id
// Если преподаватель не найден возвращает FuncError::NOT_FOUND
FuncResult <Teacher> teacherByTGID (const std::string& TG_id) {
    auto &db = Database::getInstance();

    if (!db.get_conn()) {
        return {FuncError::DB_NOT_OPEN, std::nullopt};
    }

    const char* sql = R"(
        SELECT Id, Name, Login FROM Teachers
        WHERE TG_id = ?
    )";

    sqlite3_stmt* stmt = nullptr;
    auto rc = sqlite3_prepare_v2(db.get_conn(), sql, -1, &stmt, nullptr);

    sqlite3_bind_text(stmt, 1, TG_id.c_str(), -1, SQLITE_STATIC);

    Teacher teacher;

    rc = sqlite3_step(stmt);
    
    if (rc == SQLITE_ROW) {
        int teacherId = sqlite3_column_int(stmt, 0);
        const unsigned char* name = sqlite3_column_text(stmt, 1);
        const unsigned char* login = sqlite3_column_text(stmt, 2);
        
        teacher.setId(teacherId);
        teacher.setName(std::string(reinterpret_cast<const char*>(name)));
        teacher.setLogin(std::string(reinterpret_cast<const char*>(login)));
        teacher.setUsernameTg(TG_id);
    } 

    else if (rc == SQLITE_DONE) {
        sqlite3_finalize(stmt);
        return {FuncError::NOT_FOUND, std::nullopt};
    }

    else {
        sqlite3_finalize(stmt);
        return {FuncError::STEP_FAILED, std::nullopt};
    }
    
    sqlite3_finalize(stmt);

    return {FuncError::OK, teacher};
}

// Создает одну кнопку
TgBot::InlineKeyboardButton::Ptr createBtn(const std::string& text, const std::string& callbackData) {
    auto btn = std::make_shared<TgBot::InlineKeyboardButton>();
    btn->text = text;
    btn->callbackData = callbackData;
    return btn;
}

// Создает клавиатуру из списка пар {Текст, CallbackData}
TgBot::InlineKeyboardMarkup::Ptr createKeyboard(const std::vector<std::pair<std::string, std::string>>& buttonsData, bool asRows) {
    auto keyboard = std::make_shared<TgBot::InlineKeyboardMarkup>();
    
    for (const auto& data : buttonsData) {
        std::vector<TgBot::InlineKeyboardButton::Ptr> row;
        row.push_back(createBtn(data.first, data.second));
        keyboard->inlineKeyboard.push_back(row);
    }
    return keyboard;
}

// Вспомогательная функция для создания кнопок управления очередью (в одну строку)
TgBot::InlineKeyboardMarkup::Ptr createQueueControls(int subjectId, bool isInQueue) {
    auto keyboard = std::make_shared<TgBot::InlineKeyboardMarkup>();
    std::vector<TgBot::InlineKeyboardButton::Ptr> row;

    std::string sId = std::to_string(subjectId);
    
    if (isInQueue) {
        // Если студент в очереди, показываем "Выйти"
        row.push_back(createBtn("Выйти", "leave_" + sId));
    } else {
        // Иначе показываем "Записаться"
        row.push_back(createBtn("Записаться", "join_" + sId));
    }
    
    row.push_back(createBtn("Обновить ⟳", "view_" + sId)); 

    row.push_back(createBtn("Сдвиг на 1 позицию назад", "skip_" + sId));

    keyboard->inlineKeyboard.push_back(row);
    
    // Кнопка "Назад" отдельной строкой
    std::vector<TgBot::InlineKeyboardButton::Ptr> rowBack;
    rowBack.push_back(createBtn("« К списку предметов", "role_student"));
    keyboard->inlineKeyboard.push_back(rowBack);

    return keyboard;
}

// Кнопки управления для преподователя
TgBot::InlineKeyboardMarkup::Ptr createTeacherQueueControls(int subjectId) {
    auto keyboard = std::make_shared<TgBot::InlineKeyboardMarkup>();
    std::string sId = std::to_string(subjectId);

    // 1 строка
    std::vector<TgBot::InlineKeyboardButton::Ptr> row;
    row.push_back(createBtn("Обновить ⟳", "view_" + sId)); 
    keyboard->inlineKeyboard.push_back(row);
    
    // 2 строка
    std::vector<TgBot::InlineKeyboardButton::Ptr> rowBack;
    rowBack.push_back(createBtn("« К списку предметов", "role_teacher"));
    keyboard->inlineKeyboard.push_back(rowBack);

    return keyboard;
}