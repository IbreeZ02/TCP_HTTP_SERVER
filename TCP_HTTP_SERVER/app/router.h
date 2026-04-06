#pragma once
#include "httpRequest.h"
#include "httpResponse.h"
#include "platform.h"

class Router {
public:
    HttpResponse handle(const HttpRequest& req, socket_t fd);
};