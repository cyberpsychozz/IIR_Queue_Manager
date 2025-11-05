#pragma once
#include <sqlite3.h>
#include <string>
#include <vector>
#include "Student.h"

class Database {
private:
    sqlite3* conn;
    std::string path;

public:
    Database(const std::string& path);
    ~Database();
    bool open();
    void close();
    void createTables();  // CREATE TABLE students(id INTEGER PRIMARY KEY, ...), queues(id PK, subject_id, student_id, position), etc.

    // Методы для классов: e.g.,
    void insertStudent(const Student& student);
    Student getStudentById(int id);
    std::vector<Student> getAllStudents();

    // Для Queue: custom queries (prepared statements для скорости)
    int executeIntQuery(const std::string& sql, ...);  // Для скалярных значений
    // И т.д.
};