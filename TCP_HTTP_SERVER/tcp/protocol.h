#pragma once
#include <cstdint>
#include <string>
#include "platform.h"

//guarantess all bytes are sent
int send_all(int fd, const char* buffer, int len);
int recv_all(int fd, char* buffer, int len);

//message_framing
int send_message(int fd, const std::string& msg);
std::string recv_message(int fd, uint32_t max_size = 10 * 1024 * 1024);
