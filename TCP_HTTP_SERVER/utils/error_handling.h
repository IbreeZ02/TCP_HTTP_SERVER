#pragma once
#include <stdexcept>
#include <cerrno>
#include <cstring>
#include <string>
#include "logging.h"

enum ErrorType {
    FATAL,
    RECOVERABLE
};

inline void handle_error(const std::string& message, ErrorType type = FATAL) {
    char buffer[256];
    int err = errno;

#ifdef _WIN32
    strerror_s(buffer, sizeof(buffer), err);
#else
    // Use POSIX strerror_r (returns int)
    (void)strerror_r(err, buffer, sizeof(buffer));
#endif

    std::string full_message = message + ": " + buffer;

    if (type == FATAL) {
        log_error("FATAL: " + full_message);
        throw std::runtime_error(full_message);
    }
    else {
        log_warning("RECOVERABLE: " + full_message);
    }
}