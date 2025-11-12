#include "Student.h"
#include "Database.h"
#include <optional>
#include "Error.h"
#include <vector>

// Queue(int subject_id, Database& database) : _Subject_id(subject_id), db(database) {}

inline bool is_connection_open(sqlite3* conn) {
    return conn != nullptr;
}



FuncResult<int>Queue::push(int student_id){
    if (!db.get_conn()){
        return{FuncError::DB_NOT_OPEN, std::nullopt};
    }
    
    auto LenRes = getLen();
    if (LenRes != FuncError::OK){
        return {LenRes.first, std::nullopt};
    }

    int new_pos = LenRes.second.value() + 1;

    const char* sql = "INSERT INTO Queues (Student_Id, Subject_Id, Position) VALUES (?, ?, ?);";

    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(db.get_conn(), sql, -1, &stmt, nullptr) != SQLITE_OK) {
        return {FuncError::PREPARE_FAILED, std::nullopt};
    }

    sqlite3_bind_int(stmt, 1, student_id);
    sqlite3_bind_int(stmt, 2, _Subject_id);
    sqlite3_bind_int(stmt, 3, new_pos);

    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    if(rc != SQLITE_DONE){
        return{FuncError::STEP_FAILED, std::nullopt};
    }
    
    return{FuncError::OK, new_pos};
    
}
FuncResult<bool>Queue::pop(){
    if (!db.get_conn()){
        return{FuncError::DB_NOT_OPEN, std::nullopt};
    }
    
    const char* sql_del = "DELETE FROM Queues WHERE Subject_Id = ? AND Position = 1;";
    const char* sql_shift = "UPDATE Queues SET Position = Position - 1 WHERE Subject_Id = ?;";


    sqlite3_stmt* stmt;
    // Удаление студента
    if (sqlite3_prepare_v2(db.get_conn(), sql_del, -1, &stmt, nullptr) != SQLITE_OK) {
        return {FuncError::PREPARE_FAILED, std::nullopt};
    }
    sqlite3_bind_int(stmt, 1, _Subject_id);
    if (sqlite3_step(stmt) != SQLITE_DONE){
        // не выполнился step
        sqlite3_finalize(stmt);
        return {FuncError::STEP_FAILED, std::nullopt};
    }
    sqlite3_finalize(stmt);
    // Обновление очереди
    if (sqlite3_prepare_v2(db.get_conn(), sql_shift, -1, &stmt, nullptr) != SQLITE_OK) {
        return {FuncError::PREPARE_FAILED, std::nullopt};
    }
    sqlite3_bind_int(stmt, 1, _Subject_id);
    int rc = sqlite3_step(stmt)
    sqlite3_finalize(stmt);
    
    return{FuncError::OK, rc == SQLITE_DONE};
}

// Обмен позициями в очереди (служебный или по согласию)
FuncResult<bool>Queue::swap(int pos1, int pos2){
    if (!db.get_conn()){
        return{FuncError::DB_NOT_OPEN, std::nullopt};
    }

    if(pos1 <= 0 || pos2 <= 0 || pos1 == pos2){
        return{FuncError::INVALID_POSITION, std::nullopt};
    }

    const char* sql = R"(
        UPDATE Queues SET Position = CASE
            WHEN Position = ? THEN ?
            WHEN Position = ? THEN ?
        END
        WHERE Subject_Id = ? AND Position IN (?, ?);
    )";

    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db.get_conn(), sql, -1, &stmt, nullptr) != SQLITE_OK) {
        return {FuncError::PREPARE_FAILED, std::nullopt};
    }

    sqlite3_bind_int(stmt, 1, pos1); sqlite3_bind_int(stmt, 2, pos2);
    sqlite3_bind_int(stmt, 3, pos2); sqlite3_bind_int(stmt, 4, pos1);
    sqlite3_bind_int(stmt, 5, _Subject_id);
    sqlite3_bind_int(stmt, 6, pos1); sqlite3_bind_int(stmt, 7, pos2);

    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    
    
    return{FuncError::OK, rc == SQLITE_DONE && sqlite3_changes(db.get_conn()) == 2};
}

// Пропуск одного человека вперёд
FuncResult<bool>Queue::skip(){
    return swap(1, 2);
} 
int give_up(int student_id); // Удаление из очереди по желанию Студента
// std_optional 
int getPosition(int student_id) const; // Своя позиция в очереди

FuncResult<int> Queue::getLen() const {
    if (!is_open(db.get_conn())) return {FuncError::DB_NOT_OPEN, std::nullopt};

    const char* sql = "SELECT COUNT(*) FROM Queues WHERE Subject_Id = ?;";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db.get_conn(), sql, -1, &stmt, nullptr) != SQLITE_OK)
        return {FuncError::PREPARE_FAILED, std::nullopt};

    sqlite3_bind_int(stmt, 1, _Subject_id);

    int len = 0;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        len = sqlite3_column_int(stmt, 0);
    }
    sqlite3_finalize(stmt);

    return {FuncError::OK, len};
} // Длина очереди



FuncResult<std::vector<Student>>Queue::getAllStudents(const Queue& queue){
    // hui
    std::vector<Student> students;

    // Проверяем, что соединение открыто
    if (!db.get_conn()) {
        return {FuncError::CONNECTION_CLOSED, std::nullopt};
    }

    // Подготавливаем SQL-запрос
    const char* sql = R"(
        SELECT Q.Position, Q.Student_Id, S.name 
        FROM Queues Q 
        JOIN Students S ON S.Id = Q.Student_Id 
        WHERE Q.Subject_Id = ?
    )";

    sqlite3_stmt* stmt = nullptr;
    auto rc = sqlite3_prepare_v2(db.get_conn(), sql, -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        std::string err = "SQL prepare error: ";
        err += sqlite3_errmsg(db.get_conn());
        sqlite3_finalize(stmt);
        return {FuncError::PREPARE_FAILED, std::nullopt}; //обернуть в трайкэтч
    }

    if (sqlite3_bind_int(stmt, 1, _Subject_id) != SQLITE_OK) {
        sqlite3_finalize(stmt);
        return {FuncError::BIND_FAILED, std::nullopt};
    }
    // Привязываем subject_id
    sqlite3_bind_int(stmt, 1, _Subject_id);
    Student student;
    // Выполняем шаг за шагом
    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        
        
        std::optional<const char*> name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        student.setName(*name);

        students.push_back(student);
    }
// fmtlog 
    if (rc != SQLITE_DONE) {
        std::string err = "SQL step error: ";
        err += sqlite3_errmsg(db.get_conn());
        sqlite3_finalize(stmt);
        throw {FuncError::STEP_FAILED, std::nullopt};
    }

    sqlite3_finalize(stmt);
    return {FuncError::OK, students};
}
