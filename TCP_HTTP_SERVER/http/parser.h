#pragma once
#include "httpRequest.h"
#include <string>

class Parser {
public:
    HttpRequest parse(const std::string& raw);

private:
    static std::string trim(const std::string& s);
    static std::string to_lower(std::string s);
};