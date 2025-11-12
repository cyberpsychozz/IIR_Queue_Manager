#include "Subject.h"

Subject::Subject() : id(0), name(""), teacher_id(0) {}

Subject::Subject(int subject_id, std::string name, int teacher_id)
    : id(subject_id), name(name), teacher_id(teacher_id) {}

// Getters
int Subject::getId() const { return id; }
int Subject::getTeacherId() const { return teacher_id; }
const std::string& Subject::getName() const { return name; }

// Setters
void Subject::setId(int newId) { id = newId; }
void Subject::setTeacherId(int newTeacherId) { teacher_id = newTeacherId; }
void Subject::setName(const std::string& newName) { name = newName; }

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

    std::string table_name = "Sems_" + std::to_string(Subject::id);
    std::string sql = "SELECT Id, Date, Comment FROM " + table_name + " WHERE Groups = ?";

    sqlite3_stmt* stmt = nullptr;
    auto rc = sqlite3_prepare_v2(db.get_conn(), sql.c_str(), -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        sqlite3_finalize(stmt);
        return {FuncError::PREPARE_FAILED, std::nullopt};
    }

    sqlite3_bind_int(stmt, 1, group);

    std::vector<Seminar> sems;
    Seminar sem;
    int id = 1;
    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        sem.id = id;

        const char* date = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        sem.date = date;

        const char* comment = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        sem.comment = comment ? comment : "";
        
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

FuncError addClass(const Seminar& seminar, Database& db) {

}

FuncError deleteClass(const Seminar& seminar, Database& db) {

}