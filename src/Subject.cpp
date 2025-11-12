#include "Subject.h"

FuncResult<Teacher> Subject::getTeacher(Database& db) const {
    // Проверяем, что соединение открыто
    if (!db.get_conn()) {
        return {FuncError::CONNECTION_CLOSED, std::nullopt};
    }

    // SQLite запрос
    const char* sql = R"(
        SELECT T.Id, T.name, T.Login, T.TG_id
        FROM Subjects S
        JOIN Teachers T ON T.Id = S.Teacher_Id
        WHERE S.Id = ?
    )";

    sqlite3_stmt* stmt = nullptr;
    auto rc = sqlite3_prepare_v2(db.get_conn(), sql, -1, &stmt, nullptr);

    sqlite3_bind_int(stmt, 1, id);

    Teacher prepod;

    rc = sqlite3_step(stmt);
    
    // Вернулась строка
    if (rc == SQLITE_ROW) {
        int teacherId = sqlite3_column_int(stmt, 0);
        const unsigned char* name = sqlite3_column_text(stmt, 1);
        const unsigned char* login = sqlite3_column_text(stmt, 2);
        const unsigned char* tg_id = sqlite3_column_text(stmt, 3);
        
        prepod.setId(teacherId);
        prepod.setName(std::string(reinterpret_cast<const char*>(name)));
        prepod.setLogin(std::string(reinterpret_cast<const char*>(login)));
        if (tg_id != nullptr) {
            prepod.setUsernameTg(std::string(reinterpret_cast<const char*>(tg_id)));
        }
        // else {
        //     Запросить TG_ID
        // }
    } 
    // Не вернулся результат    
    else if (rc == SQLITE_DONE) {
        sqlite3_finalize(stmt);
        return {FuncError::NOT_FOUND, std::nullopt};
    }
    // Вернулась ошибка 
    else {
        sqlite3_finalize(stmt);
        return {FuncError::STEP_FAILED, std::nullopt};
    }
    
    sqlite3_finalize(stmt);

    return {FuncError::OK, prepod};
}

FuncResult<std::vector<Seminar>> Subject::getClasses(Database& db, int group) const {
    if (!db.get_conn()) {
        return {FuncError::CONNECTION_CLOSED, std::nullopt};
    }

    const char* sql = R"(
        SELECT Id, Date, Comment
        FROM ?
        WHERE Groups = ?
    )";

    sqlite3_stmt* stmt = nullptr;
    auto rc = sqlite3_prepare_v2(db.get_conn(), sql, -1, &stmt, nullptr);

    if (rc != SQLITE_OK) {
        std::string err = "SQL prepare error: ";
        err += sqlite3_errmsg(db.get_conn());
        sqlite3_finalize(stmt);
        return {FuncError::PREPARE_FAILED, std::nullopt};
    }

    sqlite3_bind_text(stmt, 1, "Sems_" + (Subject::id + '0'), -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 1, group);

    std::vector<Seminar> sems;
    Seminar sem;
    int id = 1;

    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        sem.id = id;
        sem.date = (time_t)sqlite3_column_text(stmt, 1);

        std::optional<const char*> comment = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        sem.comment = *comment;

        sems.push_back(sem);
        ++id;
    }

    if (rc != SQLITE_DONE) {
        sqlite3_finalize(stmt);
        return {FuncError::STEP_FAILED, std::nullopt};
    }

    sqlite3_finalize(stmt);
    return {FuncError::OK, sems};
}