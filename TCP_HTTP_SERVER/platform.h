#pragma once
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#ifdef _WIN32
//#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")
using socket_t = SOCKET;
inline int close(SOCKET fd) { return closesocket(fd); }
#define ssize_t int
#else
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
using socket_t = int;
#endif
