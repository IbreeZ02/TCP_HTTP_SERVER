#pragma once
#include "httpResponse.h"
#include "httpRequest.h"
#include <string>

class FileHandler {
public:
    static HttpResponse serve(const std::string& path);
    static HttpResponse upload(const HttpRequest& req);
    static HttpResponse download(const std::string& path);
};
