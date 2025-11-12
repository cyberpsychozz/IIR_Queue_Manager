#pragma once
#include <optional>

enum class FuncError { // TODO Надо в нормальный вид этот файл привести
    OK,
    DB_NOT_OPEN,
    PREPARE_FAILED,
    BIND_FAILED,
    STEP_FAILED,
    NOT_FOUND,
    INVALID_POSITION,
    STUDENT_NOT_IN_QUEUE,
    UNKNOWN,
    CONNECTION_CLOSED
};

template<typename T>
using FuncResult = std::pair<FuncError, std::optional<T>>;

template<typename T>
bool operator==(const FuncResult<T>& result, FuncError error) {
    return result.first == error;
}

template<typename T>
bool operator!=(const FuncResult<T>& result, FuncError error) {
    return result.first != error;
}

template<typename T>
bool operator==(FuncError error, const FuncResult<T>& result) {
    return result.first == error;
}

template<typename T>
bool operator!=(FuncError error, const FuncResult<T>& result) {
    return result.first != error;
}