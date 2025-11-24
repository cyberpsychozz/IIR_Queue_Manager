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

// Возвращает данные преподавателя данного предмета
FuncResult<Teacher> Subject::getTeacher() const {
    auto &db = Database::getInstance();

    // Проверяем, что соединение открыто
    if (!db.get_conn()) {
        return {FuncError::DB_NOT_OPEN, std::nullopt};
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

// Возвращает список назначенных заданной группе семинаров по предмету
FuncResult<std::vector<Seminar>> Subject::getClasses(int group) const {
    auto &db = Database::getInstance();

    if (!db.get_conn()) {
        return {FuncError::DB_NOT_OPEN, std::nullopt};
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

// Добавляет семинар по предмету в расписание заданной группы
FuncError Subject::addClass(const Seminar& seminar, int group) {
    auto &db = Database::getInstance();

    if (!db.get_conn()) {
        return FuncError::DB_NOT_OPEN;
    }

    std::string table_name = "Sems_" + std::to_string(Subject::id);
    std::string sql = "INSERT INTO " + table_name + " (Id, Date, Comment, Groups) VALUES ((SELECT max(Id) from Sems_1) + 1, ?, ?, ?)";

    sqlite3_stmt* stmt = nullptr;
    auto rc = sqlite3_prepare_v2(db.get_conn(), sql.c_str(), -1, &stmt, nullptr);

    sqlite3_bind_text(stmt, 1, seminar.date.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, seminar.comment.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 3, group);

    rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    
    if(rc != SQLITE_DONE){
        return FuncError::STEP_FAILED;
    }

    return FuncError::OK;
}

// Удаляет семинар из расписания предмета по его id
FuncError Subject::deleteClass(int sem_id) {
    auto &db = Database::getInstance();

    if (!db.get_conn()) {
        return FuncError::DB_NOT_OPEN;
    }

    std::string table_name = "Sems_" + std::to_string(Subject::id);
    std::string sql_del = "DELETE FROM " + table_name + " WHERE Id = ?;";
    std::string sql_shift = "UPDATE " + table_name + " SET Id = Id - 1 WHERE Id > ?;";

    sqlite3_stmt* stmt = nullptr;
    // Удаление занятия
    if (sqlite3_prepare_v2(db.get_conn(), sql_del.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        return FuncError::PREPARE_FAILED;
    }

    sqlite3_bind_int(stmt, 1, sem_id);

    if (sqlite3_step(stmt) != SQLITE_DONE){
        // не выполнился step
        sqlite3_finalize(stmt);
        return FuncError::STEP_FAILED;
    }

    sqlite3_finalize(stmt);

    // Обновление очереди
    if (sqlite3_prepare_v2(db.get_conn(), sql_shift.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        return FuncError::PREPARE_FAILED;
    }

    sqlite3_bind_int(stmt, 1, sem_id);

    if (sqlite3_step(stmt) != SQLITE_DONE) {
        // не выполнился step
        sqlite3_finalize(stmt);
        return FuncError::STEP_FAILED;
    }

    sqlite3_finalize(stmt);
    
    return FuncError::OK;
}

// Добавляет данный предмет в таблицу заданным группам
FuncResult<int> Subject::addSubject(std::string groups) {
    auto &db = Database::getInstance();
    int subjectId;

    if (!db.get_conn()) {
        return {FuncError::DB_NOT_OPEN, std::nullopt};
    }

    std::string sql = "INSERT INTO Subjects (Id, Name, Teacher_Id, Groups) VALUES ((SELECT max(Id) from Subjects) + 1, ?, ?, ?)";

    sqlite3_stmt* stmt = nullptr;

    if (sqlite3_prepare_v2(db.get_conn(), sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        return {FuncError::PREPARE_FAILED, std::nullopt};
    }

    sqlite3_bind_text(stmt, 1, Subject::name.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 2, Subject::teacher_id);
    sqlite3_bind_text(stmt, 3, groups.c_str(), -1, SQLITE_STATIC);

    auto rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    
    if(rc != SQLITE_DONE){
        return {FuncError::STEP_FAILED, std::nullopt};
    }


    std::string sql_find = "SELECT max(Id) from Subjects";

    if (sqlite3_prepare_v2(db.get_conn(), sql_find.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        return {FuncError::PREPARE_FAILED, std::nullopt};
    }

    rc = sqlite3_step(stmt);

    if (rc == SQLITE_ROW) {
        subjectId = sqlite3_column_int(stmt, 0);
        sqlite3_finalize(stmt);
    } 

    else {
        sqlite3_finalize(stmt);
        return {FuncError::STEP_FAILED, std::nullopt};
    }

    Subject::id = subjectId;

    return {FuncError::OK, subjectId};
}

// Удаляет предмет из таблицы
FuncError Subject::deleteSubject() {
    auto &db = Database::getInstance();
    sqlite3_stmt* stmt = nullptr;
    int subjectId;

    if (!db.get_conn()) {
        return FuncError::DB_NOT_OPEN;
    }

    if (Subject::id != 0) {
        subjectId = Subject::id;
    }
    else {
        std::string sql_find = R"(SELECT Id FROM Subjects
                               WHERE Name = ?)";

        if (sqlite3_prepare_v2(db.get_conn(), sql_find.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
            return FuncError::PREPARE_FAILED;
        }

        sqlite3_bind_text(stmt, 1, Subject::name.c_str(), -1, SQLITE_STATIC);

        auto rc = sqlite3_step(stmt);

        // Вернулась строка
        if (rc == SQLITE_ROW) {
            id = sqlite3_column_int(stmt, 0);
            sqlite3_finalize(stmt);
        } 
        // Предмет не найден
        else if (rc == SQLITE_DONE) {
            sqlite3_finalize(stmt);
            return FuncError::NOT_FOUND;
        }
        // Вернулась ошибка 
        else {
            sqlite3_finalize(stmt);
            return FuncError::STEP_FAILED;
        }
    }

    std::string sql_del = "DELETE FROM Subjects WHERE Id = ?;";
    std::string sql_shift = "UPDATE Subjects SET Id = Id - 1 WHERE Id > ?;";

    // Удаление предмета
    if (sqlite3_prepare_v2(db.get_conn(), sql_del.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        return FuncError::PREPARE_FAILED;
    }

    sqlite3_bind_int(stmt, 1, subjectId);

    if (sqlite3_step(stmt) != SQLITE_DONE){
        sqlite3_finalize(stmt);
        return FuncError::STEP_FAILED;
    }

    sqlite3_finalize(stmt);

    // Обновление очереди
    if (sqlite3_prepare_v2(db.get_conn(), sql_shift.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        return FuncError::PREPARE_FAILED;
    }

    sqlite3_bind_int(stmt, 1, subjectId);

    if (sqlite3_step(stmt) != SQLITE_DONE) {
        sqlite3_finalize(stmt);
        return FuncError::STEP_FAILED;
    }

    sqlite3_finalize(stmt);
    
    return FuncError::OK;
}