#include "network.h"

int safe_bind(socket_t fd, sockaddr_in& address) {
    int result = ::bind(fd, (sockaddr*)&address, sizeof(address));
    if (result < 0)
        handle_error("bind() failed", FATAL);
    return result;
}

int safe_listen(socket_t fd, int backlog) {
    int result = ::listen(fd, backlog);
    if (result < 0)
        handle_error("listen() failed", FATAL);
    return result;
}


socket_t safe_accept(socket_t fd, sockaddr_in& address) {
    socklen_t len = sizeof(address);
    socket_t client_fd = ::accept(fd, (sockaddr*)&address, &len);
#ifdef _WIN32
    if (client_fd == INVALID_SOCKET) {
#else
    if (client_fd < 0) {
#endif
        handle_error("accept() failed", RECOVERABLE);
        return -1;
    }
    return client_fd;
}

int safe_send(socket_t fd, const std::string& msg) {
    //int result = send_message(fd, msg);
    int result = send_all(fd, msg.data(), (int)msg.size()); //row data for SSE, and not lenght prefix
    if (result < 0) {
        handle_error("send() failed", RECOVERABLE);
        return -1;
    }
    return result;
}

std::string safe_recv(socket_t fd) {
    std::string result = recv_message(fd);
    if (result.empty()) {
        log_info("client disconnected or error");
        return "";
    }
    return result;
}

void safe_close(socket_t fd) {
    if (::close(fd) < 0)
        handle_error("close() failed", RECOVERABLE);
}
