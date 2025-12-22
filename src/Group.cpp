#include "Group.h"

// Constructor
Group::Group(int groupId) : group_id(groupId) {}

// Functions

/*
Возвращает список студентов группы
Производит поиск по id группы
*/
FuncResult<std::vector<Student>> Group::getStudents() const {
    auto &db = Database::getInstance();

    if (!db.get_conn())
        return {FuncError::DB_NOT_OPEN, std::nullopt};

    const char* sql = R"(
        SELECT *
        FROM Students
        WHERE Groups = ?
    )";

    sqlite3_stmt* raw_stmt = nullptr;
    auto rc = sqlite3_prepare_v2(db.get_conn(), sql, -1, &raw_stmt, nullptr);
    StmtPtr stmt(raw_stmt);
    if (rc != SQLITE_OK)
        return {FuncError::PREPARE_FAILED, std::nullopt};

    sqlite3_bind_int(stmt.get(), 1, group_id);

    std::vector<Student> students;
    Student student;
    int id = 1;
    
    while ((rc = sqlite3_step(stmt.get())) == SQLITE_ROW) {

        student.setId(sqlite3_column_int(stmt.get(), 0));
        student.setGroupName(sqlite3_column_int(stmt.get(), 1));

        const char* name = reinterpret_cast<const char*>(sqlite3_column_text(stmt.get(), 2));
        student.setName(std::string(reinterpret_cast<const char*>(name)));

        const char* login = reinterpret_cast<const char*>(sqlite3_column_text(stmt.get(), 3));
        student.setName(std::string(reinterpret_cast<const char*>(login)));

        const unsigned char* tg_id = sqlite3_column_text(stmt.get(), 4);
        if (tg_id != nullptr)
            student.setUsernameTg(std::string(reinterpret_cast<const char*>(tg_id)));
        
        students.push_back(student);
        ++id;
    }

    if (rc != SQLITE_DONE)
        return {FuncError::STEP_FAILED, std::nullopt};

    return {FuncError::OK, students};
}


//Возвращает список предметов соответствующий id группы
FuncResult<std::vector<Subject>> Group::getSubjects() const {
    auto &db = Database::getInstance();
    
    if (!db.get_conn())
        return {FuncError::DB_NOT_OPEN, std::nullopt};

    const char* sql = R"(
        SELECT Id, Name, Teacher_Id
        FROM Subjects
        WHERE instr(Groups, ?) > 0;
    )";

    sqlite3_stmt* raw_stmt = nullptr;
    auto rc = sqlite3_prepare_v2(db.get_conn(), sql, -1, &raw_stmt, nullptr);
    StmtPtr stmt(raw_stmt);
    if (rc != SQLITE_OK)
        return {FuncError::PREPARE_FAILED, std::nullopt};

    sqlite3_bind_int(stmt.get(), 1, group_id);

    std::vector<Subject> subjects;
    Subject subj;
    int id = 1;
    
    while ((rc = sqlite3_step(stmt.get())) == SQLITE_ROW) {

        subj.setId(id);
        subj.setTeacherId(sqlite3_column_int(stmt.get(), 2));

        const char* name = reinterpret_cast<const char*>(sqlite3_column_text(stmt.get(), 2));
        subj.setName(std::string(reinterpret_cast<const char*>(name)));
        
        subjects.push_back(subj);
        ++id;
    }

    if (rc != SQLITE_DONE)
        return {FuncError::STEP_FAILED, std::nullopt};

    return {FuncError::OK, subjects};
}

/*
Приписывает группу к СУЩЕСТВУЮЩЕМУ предмету
Если задан аргумент subject_id производится добавление по id предмета (рекомендуется)
Если задан name - сначала поиск id предмета по названию, а затем добавление к первому найденному предмету
Если предмет отсутствует в списке или не задан ни один из аргументов, возвращается NOT FOUND
*/
FuncError Group::addToSubject(std::optional<const std::string> name, std::optional<int> subject_id) {
    auto &db = Database::getInstance();
    sqlite3_stmt* raw_stmt = nullptr;
    int id;

    if (!db.get_conn())
        return FuncError::DB_NOT_OPEN;

    if (subject_id.has_value())
        id = subject_id.value();

    else if (name.has_value()) {
        std::string sql_find = R"(SELECT Id FROM Subjects
                               WHERE Name = ?)";

        if (sqlite3_prepare_v2(db.get_conn(), sql_find.c_str(), -1, &raw_stmt, nullptr) != SQLITE_OK)
            return FuncError::PREPARE_FAILED;
        StmtPtr stmt(raw_stmt);

        sqlite3_bind_text(stmt.get(), 1, name.value().c_str(), -1, SQLITE_STATIC);

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

    else return FuncError::NOT_FOUND;

    std::string sql_update = R"(UPDATE Subjects
                                SET Groups = Groups || ?
                                WHERE Id = ?;)";

    if (sqlite3_prepare_v2(db.get_conn(), sql_update.c_str(), -1, &raw_stmt, nullptr) != SQLITE_OK)
        return FuncError::PREPARE_FAILED;
    StmtPtr stmt(raw_stmt);

    std::string group = ", " + std::to_string(Group::group_id);
    sqlite3_bind_text(stmt.get(), 1, group.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt.get(), 2, id);

    if (sqlite3_step(stmt.get()) != SQLITE_DONE)
        return FuncError::STEP_FAILED;
    
    return FuncError::OK;
}

/*
Удаляет группу из списка групп СУЩЕСТВУЮЩЕГО предмета
Если задан аргумент subject_id производится удаление по id предмета (рекомендуется)
Если задан name - сначала поиск id предмета по названию, а затем удаление у первого найденного предмета
Если предмет отсутствует в списке или не задан ни один из аргументов, возвращается NOT FOUND
*/
FuncError Group::deleteFromSubject(std::optional<const std::string> name, std::optional<int> subject_id) {
    auto &db = Database::getInstance();
    sqlite3_stmt* raw_stmt = nullptr;
    int id;

    if (!db.get_conn())
        return FuncError::DB_NOT_OPEN;

    if (subject_id.has_value())
        id = subject_id.value();

    else if (name.has_value()) {
        std::string sql_find = R"(SELECT Id FROM Subjects
                               WHERE Name = ?)";

        if (sqlite3_prepare_v2(db.get_conn(), sql_find.c_str(), -1, &raw_stmt, nullptr) != SQLITE_OK)
            return FuncError::PREPARE_FAILED;
        StmtPtr stmt(raw_stmt);

        sqlite3_bind_text(stmt.get(), 1, name.value().c_str(), -1, SQLITE_STATIC);

        auto rc = sqlite3_step(stmt.get());

        // Вернулась строка
        if (rc == SQLITE_ROW)
            id = sqlite3_column_int(stmt.get(), 0);
        // Группа не найден
        else if (rc == SQLITE_DONE)
            return FuncError::NOT_FOUND;
        // Вернулась ошибка 
        else return FuncError::STEP_FAILED;
    }

    else return FuncError::NOT_FOUND;

    std::string sql_update = R"(UPDATE Subjects
                                SET Groups = replace(Groups, ?, "")
                                WHERE Id = ?;)";

    if (sqlite3_prepare_v2(db.get_conn(), sql_update.c_str(), -1, &raw_stmt, nullptr) != SQLITE_OK)
        return FuncError::PREPARE_FAILED;
    StmtPtr stmt(raw_stmt);

    std::string group = std::to_string(Group::group_id);
    sqlite3_bind_text(stmt.get(), 1, (group + ", ").c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt.get(), 2, id);

    if (sqlite3_step(stmt.get()) != SQLITE_DONE)
        return FuncError::STEP_FAILED;

    std::string sql_update2 = R"(UPDATE Subjects
                                SET Groups = replace(Groups, ?, "")
                                WHERE Id = ?;)";

    if (sqlite3_prepare_v2(db.get_conn(), sql_update2.c_str(), -1, &raw_stmt, nullptr) != SQLITE_OK)
        return FuncError::PREPARE_FAILED;
    StmtPtr stmt2(raw_stmt);

    sqlite3_bind_text(stmt2.get(), 1, group.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt2.get(), 2, id);

    if (sqlite3_step(stmt2.get()) != SQLITE_DONE)
        return FuncError::STEP_FAILED;
    
    return FuncError::OK;
}