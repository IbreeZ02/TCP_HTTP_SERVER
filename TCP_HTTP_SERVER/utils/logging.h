#pragma once
#include <iostream>
#include <string>

inline void log_info(const std::string& message) {
    std::cout << "[INFO]  " << message << std::endl; //or '\n'
}

inline void log_error(const std::string& message) {
    std::cerr << "[ERROR] " << message << std::endl;
}

inline void log_warning(const std::string& message) {
    std::cout << "[WARN]  " << message << std::endl;
}