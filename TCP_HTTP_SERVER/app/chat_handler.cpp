#include "chat_handler.h"
#include "SSE.h"
#include "network.h"
#include <algorithm>

void ChatHandler::remove_client(socket_t fd) {
    std::lock_guard<std::mutex> lock(clients_mutex);
    clients.erase(std::remove(clients.begin(), clients.end(), fd), clients.end());
}

std::vector<socket_t> ChatHandler::clients;
std::mutex ChatHandler::clients_mutex;

HttpResponse ChatHandler::receive(const HttpRequest& req) {
    broadcast(req.body);
    HttpResponse res;
    res.status_code = 204;  // No Content - message accepted
    res.status_text = "No Content";
    return res;
}

HttpResponse ChatHandler::stream(const HttpRequest& req, socket_t fd) {
	broadcast("system: a user joined");//before push back so user won't get their own joining message
	{
		std::lock_guard<std::mutex> lock(clients_mutex);
		clients.push_back(fd); // Add client socket to list for broadcasting
	}
    // SSE headers - this opens the stream
    HttpResponse res;
    res.headers["Content-Type"] = "text/event-stream";
    res.headers["Cache-Control"] = "no-cache";
    res.headers["Connection"] = "keep-alive";
    return res;
    // socket stays open - handle_client loop keeps calling SSE::send_event
}

void ChatHandler::broadcast(const std::string& message) {
    std::lock_guard<std::mutex> lock(clients_mutex);
    for (socket_t fd : clients)
        SSE::send_event(fd, message);
}
