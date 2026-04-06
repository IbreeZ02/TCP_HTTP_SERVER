#pragma once
#include <string>
#include <unordered_map>

struct HttpResponse {
    std::string version = "HTTP/1.1";
    int status_code = 200;
    std::string status_text = "OK";
    std::unordered_map<std::string, std::string> headers;
    std::string body;
};