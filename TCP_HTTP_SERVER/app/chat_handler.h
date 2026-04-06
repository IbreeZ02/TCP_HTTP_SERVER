#pragma once
#include "httpRequest.h"
#include "httpResponse.h"
#include "platform.h"
#include <vector>
#include <mutex>

class ChatHandler {
public:
    static HttpResponse receive(const HttpRequest& req); //handles POST /send
    static HttpResponse stream(const HttpRequest& req, socket_t fd);  //handles GET /stream - fd for broadcasting
    static void broadcast(const std::string& message);
    static void remove_client(socket_t fd);

private:
    static std::vector<socket_t> clients;
    static std::mutex clients_mutex;
};