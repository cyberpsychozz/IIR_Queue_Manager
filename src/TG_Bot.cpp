#include <TG_Bot.h>

// Меню приветствия и выбора роли
void startMenu(TgBot::Bot& bot, int64_t chatId) {
    TgBot::InlineKeyboardMarkup::Ptr keyboard = std::make_shared<TgBot::InlineKeyboardMarkup>();
    
    std::vector<TgBot::InlineKeyboardButton::Ptr> row;
    row.push_back(createBtn("Студент", "role_student"));
    keyboard->inlineKeyboard.push_back(row);
    std::vector<TgBot::InlineKeyboardButton::Ptr> row2;
    row2.push_back(createBtn("Преподаватель", "role_teacher"));
    keyboard->inlineKeyboard.push_back(row2);

    try {
    bot.getApi().sendMessage(chatId, 
        "Привет! Я бот очередей ИИР.\nВыберите вашу роль:", 
        nullptr, nullptr, keyboard);
    }
    catch(const std::exception& e) { std::cerr << e.what() << '\n'; }
}

// Возвращает студента по его TG Id
// Если студент не найден возвращает FuncError::NOT_FOUND
FuncResult <Student> studentByTGID (const std::string& TG_id) {
    auto &db = Database::getInstance();

    if (!db.get_conn())
        return {FuncError::DB_NOT_OPEN, std::nullopt};

    const char* sql = R"(
        SELECT Id, Groups, Name, Login FROM Students
        WHERE TG_id = ?
    )";

    sqlite3_stmt* raw_stmt = nullptr;
    auto rc = sqlite3_prepare_v2(db.get_conn(), sql, -1, &raw_stmt, nullptr);
    StmtPtr stmt(raw_stmt);

    sqlite3_bind_text(stmt.get(), 1, TG_id.c_str(), -1, SQLITE_STATIC);

    Student student;

    rc = sqlite3_step(stmt.get());
    
    if (rc == SQLITE_ROW) {
        int studentId = sqlite3_column_int(stmt.get(), 0);
        int group = sqlite3_column_int(stmt.get(), 1);
        const unsigned char* name = sqlite3_column_text(stmt.get(), 2);
        const unsigned char* login = sqlite3_column_text(stmt.get(), 3);
        
        student.setId(studentId);
        student.setGroupName(group);
        student.setName(std::string(reinterpret_cast<const char*>(name)));
        student.setLogin(std::string(reinterpret_cast<const char*>(login)));
        student.setUsernameTg(TG_id);
    } 

    else if (rc == SQLITE_DONE)
        return {FuncError::NOT_FOUND, std::nullopt};

    else return {FuncError::STEP_FAILED, std::nullopt};

    return {FuncError::OK, student};
}

// Возвращает преподавателя по его TG Id
// Если преподаватель не найден возвращает FuncError::NOT_FOUND
FuncResult <Teacher> teacherByTGID (const std::string& TG_id) {
    auto &db = Database::getInstance();

    if (!db.get_conn())
        return {FuncError::DB_NOT_OPEN, std::nullopt};

    const char* sql = R"(
        SELECT Id, Name, Login FROM Teachers
        WHERE TG_id = ?
    )";

    sqlite3_stmt* raw_stmt = nullptr;
    auto rc = sqlite3_prepare_v2(db.get_conn(), sql, -1, &raw_stmt, nullptr);
    StmtPtr stmt(raw_stmt);

    sqlite3_bind_text(stmt.get(), 1, TG_id.c_str(), -1, SQLITE_STATIC);

    Teacher teacher;

    rc = sqlite3_step(stmt.get());
    
    if (rc == SQLITE_ROW) {
        int teacherId = sqlite3_column_int(stmt.get(), 0);
        const unsigned char* name = sqlite3_column_text(stmt.get(), 1);
        const unsigned char* login = sqlite3_column_text(stmt.get(), 2);
        
        teacher.setId(teacherId);
        teacher.setName(std::string(reinterpret_cast<const char*>(name)));
        teacher.setLogin(std::string(reinterpret_cast<const char*>(login)));
        teacher.setUsernameTg(TG_id);
    } 

    else if (rc == SQLITE_DONE)
        return {FuncError::NOT_FOUND, std::nullopt};

    else return {FuncError::STEP_FAILED, std::nullopt};

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
    std::vector<TgBot::InlineKeyboardButton::Ptr> rowMid;

    std::string sId = std::to_string(subjectId);
    
    if (isInQueue) {
        // Если студент в очереди, показываем "Выйти"
        row.push_back(createBtn("Выйти", "leave_" + sId));
        rowMid.push_back(createBtn("Сдвиг на 1 позицию вниз", "skip_" + sId));
    } else {
        // Иначе показываем "Записаться"
        row.push_back(createBtn("Записаться", "join_" + sId));
    }
    
    row.push_back(createBtn("Обновить ⟳", "view_" + sId)); 

    keyboard->inlineKeyboard.push_back(row);
    
    if (!rowMid.empty())
        keyboard->inlineKeyboard.push_back(rowMid);

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
    row.push_back(createBtn("Обновить ⟳", "tview_" + sId)); 
    keyboard->inlineKeyboard.push_back(row);

    std::vector<TgBot::InlineKeyboardButton::Ptr> row2;
    row2.push_back(createBtn("Добавить комментарий", "tcomment_" + sId));
    keyboard->inlineKeyboard.push_back(row2);

    // 2 строка
    std::vector<TgBot::InlineKeyboardButton::Ptr> rowBack;
    rowBack.push_back(createBtn("« К списку предметов", "role_teacher"));
    keyboard->inlineKeyboard.push_back(rowBack);

    return keyboard;
}

// Регистрируем студента/преподавателя в БД по логину
FuncError registrate(const std::string login, const std::string TG_Id, bool prepod) {
    auto &db = Database::getInstance();
    sqlite3_stmt* raw_stmt = nullptr;

    if (!db.get_conn())
        return FuncError::DB_NOT_OPEN;

    int Id;
    std::string sql_find;
    std::string sql_update;

    if (prepod) {
        sql_find = R"(SELECT Id FROM Teachers
                   WHERE Login = ?)";

        sql_update = R"(UPDATE Teachers
                     SET (TG_id) = (?)
                     WHERE Id = ?)";
    }
    else {
        sql_find = R"(SELECT Id FROM Students
                   WHERE Login = ?)";

        sql_update = R"(UPDATE Students
                     SET (TG_id) = (?)
                     WHERE Id = ?)";
    }


    if (sqlite3_prepare_v2(db.get_conn(), sql_find.c_str(), -1, &raw_stmt, nullptr) != SQLITE_OK)
        return FuncError::PREPARE_FAILED;
    StmtPtr find_stmt(raw_stmt);

    sqlite3_bind_text(find_stmt.get(), 1, login.c_str(), -1, SQLITE_STATIC);

    auto rc = sqlite3_step(find_stmt.get());

    // Вернулась строка
    if (rc == SQLITE_ROW)
        Id = sqlite3_column_int(find_stmt.get(), 0);
    // Не найден
    else if (rc == SQLITE_DONE)
        return FuncError::NOT_FOUND;
    // Вернулась ошибка 
    else return FuncError::STEP_FAILED;

    // Обновление
    if (sqlite3_prepare_v2(db.get_conn(), sql_update.c_str(), -1, &raw_stmt, nullptr) != SQLITE_OK)
        return FuncError::PREPARE_FAILED;
    StmtPtr upd_stmt(raw_stmt);

    sqlite3_bind_int(upd_stmt.get(), 2, Id);

    sqlite3_bind_text(upd_stmt.get(), 1, TG_Id.c_str(), -1, SQLITE_STATIC);

    if (sqlite3_step(upd_stmt.get()) != SQLITE_DONE)
        return FuncError::STEP_FAILED;

    return FuncError::OK;
}