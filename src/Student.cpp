#include "Student.h"

// Constructor
Student::Student() : id(0), group_name(0), Name(""), login(""), username_tg("") {}

// Getters

int Student::getId() const { return id; }
int Student::getGroupName() const { return group_name; }
const std::string& Student::getName() const { return Name; }
const std::string& Student::getLogin() const { return login; }
const std::string& Student::getUsernameTg() const { return username_tg; }

// Setters

void Student::setId(int newId) { id = newId; }
void Student::setGroupName(int newGroupName) { group_name = newGroupName; }
void Student::setName(const std::string& newName) { Name = newName; }
void Student::setLogin(const std::string& newLogin) { login = newLogin; }
void Student::setUsernameTg(const std::string& newUsernameTg) { username_tg = newUsernameTg; }

// Functions

// Возвращает список предметов, преподаваемых данной группе
FuncResult<std::vector<Subject>> Student::getSubjects() const {
    auto &db = Database::getInstance();

    if (!db.get_conn()) {
        return {FuncError::DB_NOT_OPEN, std::nullopt};
    }

    const char* sql = R"(
        SELECT Id, Name, Teacher_Id
        FROM Subjects
        WHERE instr(Groups, ?) > 0;
    )";

    sqlite3_stmt* stmt = nullptr;
    auto rc = sqlite3_prepare_v2(db.get_conn(), sql, -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        sqlite3_finalize(stmt);
        return {FuncError::PREPARE_FAILED, std::nullopt};
    }

    sqlite3_bind_int(stmt, 1, group_name);

    std::vector<Subject> subjects;
    Subject subj;
    int id = 1;
    
    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {

        subj.setId(id);
        subj.setTeacherId(sqlite3_column_int(stmt, 2));

        const char* name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        subj.setName(std::string(reinterpret_cast<const char*>(name)));
        
        subjects.push_back(subj);
        ++id;
    }

    if (rc != SQLITE_DONE) {
        sqlite3_finalize(stmt);
        return {FuncError::STEP_FAILED, std::nullopt};
    }
    else if (subjects.empty()) {
        sqlite3_finalize(stmt);
        return {FuncError::NOT_FOUND, std::nullopt};
    }

    sqlite3_finalize(stmt);
    return {FuncError::OK, subjects};
}

/** 
 * Добавляем или обновляем данные студента
 * Поиск происходит по логину.
 *  
 * Если задан атрибут name, то данные студента будут обновлены на данные из объекта класса
 * откуда вызывается функция. (Все данные, кроме Id)
 * 
 * Иначе студент будет добавлен в конец списка
 * 
 * ВАЖНО!!! ОБНОВЛЯЕТ Telegram_Id
*/
FuncError Student::addStudent(std::optional<std::string> login) {
    auto &db = Database::getInstance();
    sqlite3_stmt* stmt = nullptr;

    if (!db.get_conn()) {
        return FuncError::DB_NOT_OPEN;
    }

    if (login.has_value()) {

        int studentId;

        std::string sql_find = R"(SELECT Id FROM Students
                               WHERE Login = ?)";

        std::string sql_update = R"(UPDATE Students
                                 SET (TG_id) = (?)
                                 WHERE Id = ?)";

        if (sqlite3_prepare_v2(db.get_conn(), sql_find.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
            return FuncError::PREPARE_FAILED;
        }

        sqlite3_bind_text(stmt, 1, (*login).c_str(), -1, SQLITE_STATIC);

        auto rc = sqlite3_step(stmt);

        // Вернулась строка
        if (rc == SQLITE_ROW) {
            studentId = sqlite3_column_int(stmt, 0);
            sqlite3_finalize(stmt);
        } 
        // Студент не найден
        else if (rc == SQLITE_DONE) {
            sqlite3_finalize(stmt);
            return FuncError::NOT_FOUND;
        }
        // Вернулась ошибка 
        else {
            sqlite3_finalize(stmt);
            return FuncError::STEP_FAILED;
        }

        // Обновление студента
        if (sqlite3_prepare_v2(db.get_conn(), sql_update.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
            return FuncError::PREPARE_FAILED;
        }

        sqlite3_bind_int(stmt, 2, studentId);
    }

    sqlite3_bind_text(stmt, 1, Student::username_tg.c_str(), -1, SQLITE_STATIC);

    if (sqlite3_step(stmt) != SQLITE_DONE) {
        sqlite3_finalize(stmt);
        return FuncError::STEP_FAILED;
    }

    sqlite3_finalize(stmt);

    return FuncError::OK;
}

/** 
 * Удаляет студента из списка
 * Поиск происходит по id.
 *  
 * Рекомендуется напрямую задавать id, но при необходимости можно  использовать и ФИО студента.
 * 
 * Если не задан атрибут id, то он будет определён по имени.
 * Если такого студента не существует или
 * name так же не задан, то функция возвращает NOT_FOUND
*/
FuncError Student::deleteStudent(std::optional<std::string> name, std::optional<int> student_id) {
    auto &db = Database::getInstance();
    sqlite3_stmt* stmt = nullptr;
    int id;

    if (!db.get_conn()) {
        return FuncError::DB_NOT_OPEN;
    }

    if (student_id.has_value()) {
        id = student_id.value();
    }

    else if (name.has_value()) {
        std::string sql_find = R"(SELECT Id FROM Students
                               WHERE Name = ?)";

        if (sqlite3_prepare_v2(db.get_conn(), sql_find.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
            return FuncError::PREPARE_FAILED;
        }

        sqlite3_bind_text(stmt, 1, (*name).c_str(), -1, SQLITE_STATIC);

        auto rc = sqlite3_step(stmt);

        // Вернулась строка
        if (rc == SQLITE_ROW) {
            id = sqlite3_column_int(stmt, 0);
            sqlite3_finalize(stmt);
        } 
        // Студент не найден
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

    else {
        return FuncError::NOT_FOUND;
    }

    std::string sql_del = "DELETE FROM Students WHERE Id = ?;";
    std::string sql_shift = "UPDATE Students SET Id = Id - 1 WHERE Id > ?;";

    // Удаление студента
    if (sqlite3_prepare_v2(db.get_conn(), sql_del.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        return FuncError::PREPARE_FAILED;
    }

    sqlite3_bind_int(stmt, 1, id);

    if (sqlite3_step(stmt) != SQLITE_DONE){
        sqlite3_finalize(stmt);
        return FuncError::STEP_FAILED;
    }

    sqlite3_finalize(stmt);

    // Обновление списка
    if (sqlite3_prepare_v2(db.get_conn(), sql_shift.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        return FuncError::PREPARE_FAILED;
    }

    sqlite3_bind_int(stmt, 1, id);

    if (sqlite3_step(stmt) != SQLITE_DONE) {
        sqlite3_finalize(stmt);
        return FuncError::STEP_FAILED;
    }

    sqlite3_finalize(stmt);
    
    return FuncError::OK;
}