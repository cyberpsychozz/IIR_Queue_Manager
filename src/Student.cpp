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
FuncResult<std::vector<Subject>> Student::getSubjects() const {
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

    sqlite3_bind_int(stmt, 1, group_name);

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