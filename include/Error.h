#pragma once
#include <optional>

enum class FuncError {
    OK,
    DB_NOT_OPEN,
    PREPARE_FAILED,
    BIND_FAILED,
    STEP_FAILED,
    NOT_FOUND,
    NOT_FOUND,
    INVALID_POSITION,
    STUDENT_NOT_IN_QUEUE,
    INVALID_POSITION,
    STUDENT_NOT_IN_QUEUE,
    UNKNOWN,
    CONNECTION_CLOSED
};

// Удобный тип результата
template<typename T>
using FuncResult = std::pair<FuncError, std::optional<T>>;