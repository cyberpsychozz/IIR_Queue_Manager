#include "Teacher.h"

// Constructors
Teacher::Teacher() : Teacher(0, "", "", "") {}

Teacher::Teacher(int teacherId, const std::string& teacherName, 
        const std::string& teacherLogin, const std::string& teacherUsernameTg)
    : id(teacherId), name(teacherName), login(teacherLogin), username_tg(teacherUsernameTg) {}

Teacher::~Teacher() = default;

// Getters
int Teacher::getId() const { return id; }
const std::string& Teacher::getName() const { return name; }
const std::string& Teacher::getLogin() const { return login; }
const std::string& Teacher::getUsernameTg() const { return username_tg; }

//Setters
void Teacher::setId(int newId) { id = newId; }
void Teacher::setName(const std::string& newName) { name = newName; }
void Teacher::setLogin(const std::string& newLogin) { login = newLogin; }
void Teacher::setUsernameTg(const std::string& newUsernameTg) { username_tg = newUsernameTg; }

// Functions
FuncResult<std::vector<Subject>> Teacher::getSubjects(Database& db) const {
    if (!db.get_conn()) {
        return {FuncError::CONNECTION_CLOSED, std::nullopt};
    }

    const char* sql = R"(
        SELECT Id, Name, Teacher_Id
        FROM Subjects
        WHERE Teacher_Id = ?;
    )";

    sqlite3_stmt* stmt = nullptr;
    auto rc = sqlite3_prepare_v2(db.get_conn(), sql, -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        sqlite3_finalize(stmt);
        return {FuncError::PREPARE_FAILED, std::nullopt};
    }

    sqlite3_bind_int(stmt, 1, id);

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