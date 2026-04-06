#pragma once
#include "httpRequest.h"
#include "httpResponse.h"

class Connection {
public:
    bool should_keep_alive(const HttpRequest& req);
    void apply(const HttpRequest& req, HttpResponse& res);
};