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

// Functions

// Извлекает данные о предмете по его Id
FuncError Subject::sync() {
    auto &db = Database::getInstance();

    // Проверяем, что соединение открыто
    if (!db.get_conn())
        return FuncError::DB_NOT_OPEN;

    const char* sql = R"(
        SELECT *
        FROM Subjects S
        WHERE S.Id = ?
    )";

    sqlite3_stmt* raw_stmt = nullptr;
    auto rc = sqlite3_prepare_v2(db.get_conn(), sql, -1, &raw_stmt, nullptr);
    StmtPtr stmt(raw_stmt);

    sqlite3_bind_int(stmt.get(), 1, id);

    rc = sqlite3_step(stmt.get());
    
    // Вернулась строка
    if (rc == SQLITE_ROW) {
        const unsigned char* namebd = sqlite3_column_text(stmt.get(), 1);
        name = std::string(reinterpret_cast<const char*>(namebd));
        teacher_id = sqlite3_column_int(stmt.get(), 2);
    } 
    // Не вернулся результат    
    else if (rc == SQLITE_DONE)
        return FuncError::NOT_FOUND;
    // Вернулась ошибка 
    else return FuncError::STEP_FAILED;

    return FuncError::OK;
}

// Возвращает данные преподавателя данного предмета
FuncResult<Teacher> Subject::getTeacher() const {
    auto &db = Database::getInstance();

    // Проверяем, что соединение открыто
    if (!db.get_conn())
        return {FuncError::DB_NOT_OPEN, std::nullopt};

    // SQLite запрос
    const char* sql = R"(
        SELECT T.Id, T.name, T.Login, T.TG_id
        FROM Subjects S
        JOIN Teachers T ON T.Id = S.Teacher_Id
        WHERE S.Id = ?
    )";

    sqlite3_stmt* raw_stmt = nullptr;
    auto rc = sqlite3_prepare_v2(db.get_conn(), sql, -1, &raw_stmt, nullptr);
    StmtPtr stmt(raw_stmt);

    sqlite3_bind_int(stmt.get(), 1, id);

    Teacher prepod;

    rc = sqlite3_step(stmt.get());
    
    // Вернулась строка
    if (rc == SQLITE_ROW) {
        int teacherId = sqlite3_column_int(stmt.get(), 0);
        const unsigned char* name = sqlite3_column_text(stmt.get(), 1);
        const unsigned char* login = sqlite3_column_text(stmt.get(), 2);
        const unsigned char* tg_id = sqlite3_column_text(stmt.get(), 3);
        
        prepod.setId(teacherId);
        prepod.setName(std::string(reinterpret_cast<const char*>(name)));
        prepod.setLogin(std::string(reinterpret_cast<const char*>(login)));
        if (tg_id != nullptr)
            prepod.setUsernameTg(std::string(reinterpret_cast<const char*>(tg_id)));

    } 
    // Не вернулся результат    
    else if (rc == SQLITE_DONE)
        return {FuncError::NOT_FOUND, std::nullopt};
    // Вернулась ошибка 
    else  return {FuncError::STEP_FAILED, std::nullopt};

    return {FuncError::OK, prepod};
}

// Возвращает список назначенных заданной группе семинаров по предмету
FuncResult<std::vector<Seminar>> Subject::getClasses(int group) const {
    auto &db = Database::getInstance();

    if (!db.get_conn())
        return {FuncError::DB_NOT_OPEN, std::nullopt};

    std::string table_name = "Sems_" + std::to_string(Subject::id);
    std::string sql = "SELECT Id, Date, Comment FROM " + table_name + " WHERE Groups = ?";

    sqlite3_stmt* raw_stmt = nullptr;
    auto rc = sqlite3_prepare_v2(db.get_conn(), sql.c_str(), -1, &raw_stmt, nullptr);
    StmtPtr stmt(raw_stmt);
    if (rc != SQLITE_OK)
        return {FuncError::PREPARE_FAILED, std::nullopt};

    sqlite3_bind_int(stmt.get(), 1, group);

    std::vector<Seminar> sems;
    Seminar sem;
    int id = 1;
    while ((rc = sqlite3_step(stmt.get())) == SQLITE_ROW) {
        sem.id = id;

        const char* date = reinterpret_cast<const char*>(sqlite3_column_text(stmt.get(), 1));
        sem.date = date;

        const char* comment = reinterpret_cast<const char*>(sqlite3_column_text(stmt.get(), 2));
        sem.comment = comment ? comment : "";
        
        sems.push_back(sem);
        ++id;
    }

    if (rc != SQLITE_DONE)
        return {FuncError::STEP_FAILED, std::nullopt};

    return {FuncError::OK, sems};
}

// Добавляет семинар по предмету в расписание заданной группы
FuncError Subject::addClass(const Seminar& seminar, int group) {
    auto &db = Database::getInstance();

    if (!db.get_conn())
        return FuncError::DB_NOT_OPEN;

    std::string table_name = "Sems_" + std::to_string(Subject::id);
    std::string sql = "INSERT INTO " + table_name + " (Id, Date, Comment, Groups) VALUES ((SELECT max(Id) from Sems_1) + 1, ?, ?, ?)";

    sqlite3_stmt* raw_stmt = nullptr;
    auto rc = sqlite3_prepare_v2(db.get_conn(), sql.c_str(), -1, &raw_stmt, nullptr);
    StmtPtr stmt(raw_stmt);

    sqlite3_bind_text(stmt.get(), 1, seminar.date.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt.get(), 2, seminar.comment.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt.get(), 3, group);

    rc = sqlite3_step(stmt.get());
    
    if(rc != SQLITE_DONE)
        return FuncError::STEP_FAILED;

    return FuncError::OK;
}

// Удаляет семинар из расписания предмета по его id
FuncError Subject::deleteClass(int sem_id) {
    auto &db = Database::getInstance();

    if (!db.get_conn())
        return FuncError::DB_NOT_OPEN;

    std::string table_name = "Sems_" + std::to_string(Subject::id);
    std::string sql_del = "DELETE FROM " + table_name + " WHERE Id = ?;";
    std::string sql_shift = "UPDATE " + table_name + " SET Id = Id - 1 WHERE Id > ?;";

    sqlite3_stmt* raw_stmt = nullptr;
    // Удаление занятия
    if (sqlite3_prepare_v2(db.get_conn(), sql_del.c_str(), -1, &raw_stmt, nullptr) != SQLITE_OK)
        return FuncError::PREPARE_FAILED;
    StmtPtr stmt(raw_stmt);

    sqlite3_bind_int(stmt.get(), 1, sem_id);

    // не выполнился step
    if (sqlite3_step(stmt.get()) != SQLITE_DONE)
        return FuncError::STEP_FAILED;

    // Обновление очереди
    if (sqlite3_prepare_v2(db.get_conn(), sql_shift.c_str(), -1, &raw_stmt, nullptr) != SQLITE_OK)
        return FuncError::PREPARE_FAILED;
    StmtPtr stmt2(raw_stmt);

    sqlite3_bind_int(stmt2.get(), 1, sem_id);

    // не выполнился step
    if (sqlite3_step(stmt2.get()) != SQLITE_DONE)
        return FuncError::STEP_FAILED;
    
    return FuncError::OK;
}

// Добавляет данный предмет в таблицу заданным группам
FuncResult<int> Subject::addSubject(const std::string groups) {
    auto &db = Database::getInstance();
    int subjectId;

    if (!db.get_conn())
        return {FuncError::DB_NOT_OPEN, std::nullopt};

    std::string sql = "INSERT INTO Subjects (Id, Name, Teacher_Id, Groups) VALUES ((SELECT max(Id) from Subjects) + 1, ?, ?, ?)";

    sqlite3_stmt* raw_stmt = nullptr;

    if (sqlite3_prepare_v2(db.get_conn(), sql.c_str(), -1, &raw_stmt, nullptr) != SQLITE_OK)
        return {FuncError::PREPARE_FAILED, std::nullopt};
    StmtPtr stmt(raw_stmt);

    sqlite3_bind_text(stmt.get(), 1, Subject::name.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt.get(), 2, Subject::teacher_id);
    sqlite3_bind_text(stmt.get(), 3, groups.c_str(), -1, SQLITE_STATIC);

    auto rc = sqlite3_step(stmt.get());
    
    if(rc != SQLITE_DONE)
        return {FuncError::STEP_FAILED, std::nullopt};

    std::string sql_find = "SELECT max(Id) from Subjects";

    if (sqlite3_prepare_v2(db.get_conn(), sql_find.c_str(), -1, &raw_stmt, nullptr) != SQLITE_OK)
        return {FuncError::PREPARE_FAILED, std::nullopt};
    StmtPtr stmt2(raw_stmt);

    rc = sqlite3_step(stmt2.get());

    if (rc == SQLITE_ROW)
        subjectId = sqlite3_column_int(stmt2.get(), 0);

    else return {FuncError::STEP_FAILED, std::nullopt};

    Subject::id = subjectId;

    return {FuncError::OK, subjectId};
}

// Удаляет предмет из таблицы
FuncError Subject::deleteSubject() {
    auto &db = Database::getInstance();
    sqlite3_stmt* raw_stmt = nullptr;
    int subjectId;

    if (!db.get_conn())
        return FuncError::DB_NOT_OPEN;

    if (Subject::id != 0)
        subjectId = Subject::id;
    else {
        std::string sql_find = R"(SELECT Id FROM Subjects
                               WHERE Name = ?)";

        if (sqlite3_prepare_v2(db.get_conn(), sql_find.c_str(), -1, &raw_stmt, nullptr) != SQLITE_OK)
            return FuncError::PREPARE_FAILED;
        StmtPtr stmt(raw_stmt);

        sqlite3_bind_text(stmt.get(), 1, Subject::name.c_str(), -1, SQLITE_STATIC);

        auto rc = sqlite3_step(stmt.get());

        // Вернулась строка
        if (rc == SQLITE_ROW)
            id = sqlite3_column_int(stmt.get(), 0);
        // Предмет не найден
        else if (rc == SQLITE_DONE)
            return FuncError::NOT_FOUND;
        // Вернулась ошибка 
        else return FuncError::STEP_FAILED;
    }

    std::string sql_del = "DELETE FROM Subjects WHERE Id = ?;";
    std::string sql_shift = "UPDATE Subjects SET Id = Id - 1 WHERE Id > ?;";

    // Удаление предмета
    if (sqlite3_prepare_v2(db.get_conn(), sql_del.c_str(), -1, &raw_stmt, nullptr) != SQLITE_OK)
        return FuncError::PREPARE_FAILED;
    StmtPtr stmt2(raw_stmt);

    sqlite3_bind_int(stmt2.get(), 1, subjectId);

    if (sqlite3_step(stmt2.get()) != SQLITE_DONE)
        return FuncError::STEP_FAILED;

    // Обновление очереди
    if (sqlite3_prepare_v2(db.get_conn(), sql_shift.c_str(), -1, &raw_stmt, nullptr) != SQLITE_OK)
        return FuncError::PREPARE_FAILED;
    StmtPtr stmt3(raw_stmt);

    sqlite3_bind_int(stmt3.get(), 1, subjectId);

    if (sqlite3_step(stmt3.get()) != SQLITE_DONE)
        return FuncError::STEP_FAILED;
    
    return FuncError::OK;
}