#pragma once
#include <optional>

enum class QueueError {
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

// Удобный тип результата
template<typename T>
using QueueResult = std::pair<QueueError, std::optional<T>>;