#include "server.h"
#include "network.h"
#include "buffer_handling.h"
#include "server.h"
#include "network.h"
#include "buffer_handling.h"
#include "parser.h"
#include "serializer.h"
#include "router.h"
#include "connection.h"
#include "chat_handler.h"
#include <algorithm>
#include "platform.h"
#include <chrono>


void Server::start() {
	socket.socket_descriptor = ::socket(AF_INET, SOCK_STREAM, 0);
#ifdef _WIN32
    if (socket.socket_descriptor == INVALID_SOCKET)
#else
    if (socket.socket_descriptor < 0)
#endif
        handle_error("socket() failed", FATAL);
    int opt = 1;
    setsockopt(socket.socket_descriptor, SOL_SOCKET, SO_REUSEADDR, (const char*)&opt, sizeof(opt));
	//setsockopt(socket.socket_descriptor, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    bind();
    listen();
    socket.state = RUNNING;
    running = true;
    log_info("Server started on " + socket.ip + ":" + std::to_string(socket.port));
    log_info("Working directory: files served from 'static/'");
    accept();
}

//*used inside
void Server::bind() {
    sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_port = htons(socket.port);
    address.sin_addr.s_addr = inet_addr(socket.ip.c_str());
    //inet_pton(AF_INET, socket.ip.c_str(), &address.sin_addr);
    safe_bind(socket.socket_descriptor, address);
}
void Server::listen() {
    safe_listen(socket.socket_descriptor, 128);
}
void Server::accept() {
    while (running) {
        sockaddr_in client_addr;
        socket_t client_fd = safe_accept(socket.socket_descriptor, client_addr);
        if (client_fd < 0) continue;

        //timeout
        // Replace with this (30 minutes for keep-alive, applied only to non-SSE):
#ifdef _WIN32
        DWORD timeout_ms = 30 * 60 * 1000;
        setsockopt(client_fd, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeout_ms, sizeof(timeout_ms));
#else
        struct timeval tv;
        tv.tv_sec = 30 * 60;
        tv.tv_usec = 0;
        setsockopt(client_fd, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof tv);
#endif

        // extractracting ip and port
        std::string ip = inet_ntoa(client_addr.sin_addr);
        int port = ntohs(client_addr.sin_port);

        Client* client = new Client(client_fd, ip, port);
        connect(client); //adding client to connected list
        //create thread
        std::thread(&Server::handle_client, this, client).detach();
    }
}
void Server::connect(Client* client) {
    std::lock_guard<std::mutex> lock(clients_mutex);
    connected_clients.push_back(client);
    log_info("client connected: " + client->socket.ip);
}
void Server::disconnect(Client* client) {
    {
        std::lock_guard<std::mutex> lock(clients_mutex);
        connected_clients.remove(client);
    }
    ChatHandler::broadcast("system: a user left");
    
    ChatHandler::remove_client(client->socket.socket_descriptor);
    /*remove from SSE broadcast list - direct access doesn't work tho
    {
        std::lock_guard<std::mutex> lock(ChatHandler::clients_mutex);
        auto& cl = ChatHandler::clients;
        cl.erase(std::remove(cl.begin(), cl.end(),
        client->socket.socket_descriptor), cl.end());
    }*/

    safe_close(client->socket.socket_descriptor);
    log_info("client disconnected: " + client->socket.ip);
    delete client;
}

void Server::stop() {
    running = false;
    socket.state = CLOSED;
    safe_close(socket.socket_descriptor);
    std::lock_guard<std::mutex> lock(clients_mutex);
    for (Client* client : connected_clients) {
        safe_close(client->socket.socket_descriptor);
        delete client;
    }
    connected_clients.clear();
}

void Server::broadcast(const std::string& message) {
    std::lock_guard<std::mutex> lock(clients_mutex);
    for (Client* client : connected_clients)
        safe_send(client->socket.socket_descriptor, message);
}

void Server::handle_client(Client* client) {
    Parser     parser;
    Serializer serializer;
    Router     router;
    Connection connection;

    while (running) {
        std::string raw = read_until_complete(client->socket.socket_descriptor);
        if (raw.empty()) break;

        HttpRequest  req = parser.parse(raw);

        //check for Expect: 100-continue
        auto expect_it = req.headers.find("expect");
        if (expect_it != req.headers.end() && expect_it->second == "100-continue") {
            std::string continue_response = "HTTP/1.1 100 Continue\r\n\r\n";
            safe_send(client->socket.socket_descriptor, continue_response);
        }

        HttpResponse res = router.handle(req, client->socket.socket_descriptor);
        connection.apply(req, res);
        safe_send(client->socket.socket_descriptor, serializer.serialize(res));

        // SSE: keep connection open and monitor for disconnection
        if (res.headers.count("Content-Type") &&
            res.headers.at("Content-Type") == "text/event-stream") {
            char dummy;
            while (true) {
                int ret = ::recv(client->socket.socket_descriptor, &dummy, 1, MSG_PEEK);
                if (ret <= 0) break;                // client disconnected
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
            }
            disconnect(client);
            return;
        }

        if (!connection.should_keep_alive(req)) break;
    }

    disconnect(client);
}
