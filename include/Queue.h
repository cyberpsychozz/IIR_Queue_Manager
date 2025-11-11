#pragma once
#include "Student.h"
#include "Database.h"
#include <optional>
#include <vector>

class Queue{
private:
    int _Subject_id;  // Предмет очереди
    Database& db;    // Ссылка на очередь

public: 
    // Constructors

    Queue(int subject_id, Database& database) : _Subject_id(subject_id), db(database) {}

    int push(int student_id);
    int pop();
    int swap(int pos1, int pos2); // Обмен позициями в очереди (служебный или по согласию)
    int skip(); // Пропуск одного человека вперёд
    int give_up(int student_id); // Удаление из очереди по желанию Студента
// std_optional 
    int getPosition(int student_id) const; // Своя позиция в очереди
    int getLen() const;  // Длина очереди

    

    std::vector<Student> getAllStudents(const Queue& queue){
        // hui
        std::vector<Student> students;

        // Проверяем, что соединение открыто
        if (!db.get_conn()) {
            throw std::runtime_error("Database connection is not open");
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
            throw std::runtime_error(err); //обернуть в трайкэтч
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
            throw std::runtime_error(err);
        }

        sqlite3_finalize(stmt);
        return students;
    }
};