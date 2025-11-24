#pragma once
#include <optional>

// Класс ошибок функций
enum class FuncError {
    OK,                 ///< Функция завершилась нормально
    DB_NOT_OPEN,        ///< База данных закрыта
    PREPARE_FAILED,     ///< Подготовка sql-запроса завершилась некорректно
    BIND_FAILED,        ///< bind значения в запрос завершился некорректно
    STEP_FAILED,        ///< Активация запроса завершилась некорректно
    NOT_FOUND,          ///< Элемент не найден
    INVALID_POSITION,   ///< Позиции в queue.swap() некорректны
    UNKNOWN             ///< Неизвестная ошибка
};

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