#pragma once
#include "Student.h"
#include "Database.h"
#include <optional>
#include "QueueError.h"
#include <vector>

// Queue(int subject_id, Database& database) : _Subject_id(subject_id), db(database) {}

inline bool is_connection_open(sqlite3* conn) {
    return conn != nullptr;
}



std::pair<QueueError, std::optional<int>>
push(int student_id){
    if (!db.get_conn()){
        return{QueueError::CONNECTION_CLOSED, std::nullopt};
    }

    const char* sql 
}
int pop();
int swap(int pos1, int pos2); // Обмен позициями в очереди (служебный или по согласию)
int skip(); // Пропуск одного человека вперёд
int give_up(int student_id); // Удаление из очереди по желанию Студента
// std_optional 
int getPosition(int student_id) const; // Своя позиция в очереди
int getLen() const;  // Длина очереди



std::pair<QueueError, std::optional<std::vector<Student>>>
getAllStudents(const Queue& queue){
    // hui
    std::vector<Student> students;

    // Проверяем, что соединение открыто
    if (!db.get_conn()) {
        return {QueueError::CONNECTION_CLOSED, std::nullopt};
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
        return {DBError::PREPARE_FAILED, std::nullopt}; //обернуть в трайкэтч
    }

    if (sqlite3_bind_int(stmt, 1, queue._Subject_id) != SQLITE_OK) {
        sqlite3_finalize(stmt);
        return {DBError::BIND_FAILED, std::nullopt};
    }
    // Привязываем subject_id
    sqlite3_bind_int(stmt, 1, queue._Subject_id);
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
        throw {DBError::STEP_FAILED, std::nullopt};
    }

    sqlite3_finalize(stmt);
    return {DBError::OK, students};
}
