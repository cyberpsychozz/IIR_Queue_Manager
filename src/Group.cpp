#include "Group.h"

// Constructor
Group::Group(int groupId) : group_id(groupId) {}

// Functions
FuncResult<std::vector<Student>> Group::getStudents(Database& db) const {
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

        student.setId(id);
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

FuncResult<std::vector<Subject>> Group::getSubjects(Database& db) const { // FIXME Дублирует функцию студента
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