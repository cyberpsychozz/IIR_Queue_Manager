#include "Group.h"

// Constructor
Group::Group(int groupId) : group_id(groupId) {}

// Functions
FuncResult<std::vector<Student>> Group::getStudents() const {
    auto &db = Database::getInstance();

    if (!db.get_conn()) {
        return {FuncError::CONNECTION_CLOSED, std::nullopt};
    }

    const char* sql = R"(
        SELECT *
        FROM Students
        WHERE Groups = ?
    )";

    sqlite3_stmt* stmt = nullptr;
    auto rc = sqlite3_prepare_v2(db.get_conn(), sql, -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        sqlite3_finalize(stmt);
        return {FuncError::PREPARE_FAILED, std::nullopt};
    }

    sqlite3_bind_int(stmt, 1, group_id);

    std::vector<Student> students;
    Student student;
    int id = 1;
    
    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {

        student.setId(sqlite3_column_int(stmt, 0));
        student.setGroupName(sqlite3_column_int(stmt, 1));

        const char* name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        student.setName(std::string(reinterpret_cast<const char*>(name)));

        const char* login = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
        student.setName(std::string(reinterpret_cast<const char*>(login)));

        const unsigned char* tg_id = sqlite3_column_text(stmt, 4);
        if (tg_id != nullptr) {
            student.setUsernameTg(std::string(reinterpret_cast<const char*>(tg_id)));
        }
        
        students.push_back(student);
        ++id;
    }

    if (rc != SQLITE_DONE) {
        sqlite3_finalize(stmt);
        return {FuncError::STEP_FAILED, std::nullopt};
    }

    sqlite3_finalize(stmt);
    return {FuncError::OK, students};
}

FuncResult<std::vector<Subject>> Group::getSubjects() const {
    auto &db = Database::getInstance();
    
    if (!db.get_conn()) {
        return {FuncError::CONNECTION_CLOSED, std::nullopt};
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

    sqlite3_bind_int(stmt, 1, group_id);

    std::vector<Subject> subjects;
    Subject subj;
    int id = 1;
    
    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {

        subj.setId(id);
        subj.setTeacherId(sqlite3_column_int(stmt, 2));

        const char* name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        subj.setName(std::string(reinterpret_cast<const char*>(name)));
        
        subjects.push_back(subj);
        ++id;
    }

    if (rc != SQLITE_DONE) {
        sqlite3_finalize(stmt);
        return {FuncError::STEP_FAILED, std::nullopt};
    }

    sqlite3_finalize(stmt);
    return {FuncError::OK, subjects};
}

FuncError Group::addToSubject(std::optional<std::string> name, std::optional<int> subject_id) {
    auto &db = Database::getInstance();
    sqlite3_stmt* stmt = nullptr;
    int id;

    if (!db.get_conn()) {
        return FuncError::CONNECTION_CLOSED;
    }

    if (subject_id.has_value()) {
        id = subject_id.value();
    }

    else if (name.has_value()) {
        std::string sql_find = R"(SELECT Id FROM Subjects
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

    std::string sql_update = R"(UPDATE Subjects
                                SET Groups = Groups || ?
                                WHERE Id = ?;)";

    // Удаление студента
    if (sqlite3_prepare_v2(db.get_conn(), sql_update.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        return FuncError::PREPARE_FAILED;
    }

    std::string group = ", " + std::to_string(Group::group_id);
    sqlite3_bind_text(stmt, 1, group.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 2, id);

    if (sqlite3_step(stmt) != SQLITE_DONE){
        sqlite3_finalize(stmt);
        return FuncError::STEP_FAILED;
    }

    sqlite3_finalize(stmt);
    
    return FuncError::OK;
}

FuncError Group::deleteFromSubject(std::optional<std::string> name, std::optional<int> subject_id) {
    auto &db = Database::getInstance();
    sqlite3_stmt* stmt = nullptr;
    int id;

    if (!db.get_conn()) {
        return FuncError::CONNECTION_CLOSED;
    }

    if (subject_id.has_value()) {
        id = subject_id.value();
    }

    else if (name.has_value()) {
        std::string sql_find = R"(SELECT Id FROM Subjects
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
        // Группа не найден
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

    std::string sql_update = R"(UPDATE Subjects
                                SET Groups = replace(Groups, ?, "")
                                WHERE Id = ?;)";

    if (sqlite3_prepare_v2(db.get_conn(), sql_update.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        return FuncError::PREPARE_FAILED;
    }

    std::string group = std::to_string(Group::group_id);
    sqlite3_bind_text(stmt, 1, (group + ", ").c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 2, id);

    if (sqlite3_step(stmt) != SQLITE_DONE){
        sqlite3_finalize(stmt);
        return FuncError::STEP_FAILED;
    }

    sqlite3_finalize(stmt);

    std::string sql_update2 = R"(UPDATE Subjects
                                SET Groups = replace(Groups, ?, "")
                                WHERE Id = ?;)";

    if (sqlite3_prepare_v2(db.get_conn(), sql_update2.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        return FuncError::PREPARE_FAILED;
    }

    sqlite3_bind_text(stmt, 1, group.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 2, id);

    if (sqlite3_step(stmt) != SQLITE_DONE){
        sqlite3_finalize(stmt);
        return FuncError::STEP_FAILED;
    }

    sqlite3_finalize(stmt);
    
    return FuncError::OK;
}