#pragma once
#include <optional>
#include <iostream>

// Класс ошибок функций
enum class FuncError {
    OK,                 ///< Функция завершилась нормально
    DB_NOT_OPEN,        ///< База данных закрыта
    PREPARE_FAILED,     ///< Подготовка sql-запроса завершилась некорректно
    BIND_FAILED,        ///< bind значения в запрос завершился некорректно
    STEP_FAILED,        ///< Активация запроса завершилась некорректно
    NOT_FOUND,          ///< Элемент не найден
    INVALID_POSITION,   ///< Позиции в queue.swap() или queue.skip() некорректны
    UNKNOWN             ///< Неизвестная ошибка
};

inline std::ostream& operator<<(std::ostream& os, const FuncError& error) {
    switch (error) {
        case FuncError::OK:               os << "OK"; break;
        case FuncError::DB_NOT_OPEN:      os << "DB_NOT_OPEN"; break;
        case FuncError::PREPARE_FAILED:   os << "PREPARE_FAILED"; break;
        case FuncError::BIND_FAILED:      os << "BIND_FAILED"; break;
        case FuncError::STEP_FAILED:      os << "STEP_FAILED"; break;
        case FuncError::NOT_FOUND:        os << "NOT_FOUND"; break;
        case FuncError::INVALID_POSITION: os << "INVALID_POSITION"; break;
        case FuncError::UNKNOWN:          os << "UNKNOWN"; break;
    }
    return os;
}

template<typename T>
// Результат функции - пара FuncError и возвращаемого значения
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