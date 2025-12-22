#include "Teacher.h"

// Constructors

Teacher::Teacher(int teacherId, const std::string& teacherName, 
        const std::string& teacherLogin, const std::string& teacherUsernameTg)
    : id(teacherId), name(teacherName), login(teacherLogin), username_tg(teacherUsernameTg) {}

Teacher::Teacher() : Teacher(0, "", "", "") {}

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

// Возвращает расписание семинаров преподавателя
FuncResult<std::vector<Subject>> Teacher::getSubjects() const {
    auto &db = Database::getInstance();

    if (!db.get_conn())
        return {FuncError::DB_NOT_OPEN, std::nullopt};

    const char* sql = R"(
        SELECT Id, Name
        FROM Subjects
        WHERE Teacher_Id = ?;
    )";

    sqlite3_stmt* raw_stmt = nullptr;
    auto rc = sqlite3_prepare_v2(db.get_conn(), sql, -1, &raw_stmt, nullptr);
    StmtPtr stmt(raw_stmt);
    if (rc != SQLITE_OK)
        return {FuncError::PREPARE_FAILED, std::nullopt};

    sqlite3_bind_int(stmt.get(), 1, id);

    std::vector<Subject> subjects;
    Subject subj;
    
    while ((rc = sqlite3_step(stmt.get())) == SQLITE_ROW) {
        subj.setId(sqlite3_column_int(stmt.get(), 0));
        subj.setTeacherId(id);

        const char* name = reinterpret_cast<const char*>(sqlite3_column_text(stmt.get(), 1));
        subj.setName(std::string(reinterpret_cast<const char*>(name)));
        
        subjects.push_back(subj);
    }

    if (rc != SQLITE_DONE)
        return {FuncError::STEP_FAILED, std::nullopt};
    
    else if (subjects.empty())
        return {FuncError::NOT_FOUND, std::nullopt};

    return {FuncError::OK, subjects};
}