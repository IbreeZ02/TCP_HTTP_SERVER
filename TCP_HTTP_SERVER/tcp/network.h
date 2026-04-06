#pragma once
#include "protocol.h"
#include "platform.h"
#include "error_handling.h"
#include "logging.h"
#include <string>

int safe_bind(socket_t fd, sockaddr_in& address);
int safe_listen(socket_t fd, int backlog);
socket_t safe_accept(socket_t fd, sockaddr_in& address);
int safe_send(socket_t fd, const std::string& msg);
std::string safe_recv(socket_t fd);
void safe_close(socket_t fd);