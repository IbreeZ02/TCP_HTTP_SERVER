#pragma once
// http_request.h
#pragma once
#include <string>
#include <unordered_map>

struct HttpRequest {
    std::string method;   //"GET", "POST"...
    std::string path;     //"/send", "/index.html"
    std::string version;  //"HTTP/1.1"
    std::unordered_map<std::string, std::string> headers; // { "Content-Type": "application/json", "Content-Length": "15" ... }
    std::string body;     //raw body bytes (empty if GET)
    //bool keep_alive = true;

    //convenience helpers
    /*bool keep_alive() const {
        auto it = headers.find("Connection");
        if (it == headers.end()) return true; // HTTP/1.1 default
        return it->second != "close";
    }

    int content_length() const {
        auto it = headers.find("Content-Length");
        if (it == headers.end()) return 0;
        return std::stoi(it->second);
    }*/
};