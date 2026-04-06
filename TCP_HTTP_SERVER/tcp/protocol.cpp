//#include <arpa/inet.h>
//#include <sys/socket.h>
#include "protocol.h"

int send_all(int fd, const char* buffer, int len) {
    int total = 0;
    while (total < len) {
        int sent = ::send(fd, buffer + total, len - total, 0);
        if (sent < 0) return -1;
        total += sent;
    }
    return total;
}

int recv_all(int fd, char* buffer, int len) {
    int total = 0;
    while (total < len) {
        int received = ::recv(fd, buffer + total, len - total, 0);
        if (received < 0) return -1;
        if (received == 0) return 0;
        total += received;
    }
    return total;
}

int send_message(int fd, const std::string& msg) {
    uint32_t net_len = htonl(static_cast<uint32_t>(msg.size()));
    if (send_all(fd, reinterpret_cast<const char*>(&net_len), sizeof(net_len)) < 0)
        return -1;
    if (send_all(fd, msg.data(), static_cast<int>(msg.size())) < 0)
        return -1;
    return 0;
}

std::string recv_message(int fd, uint32_t max_size) {
    uint32_t net_len = 0;
    if (recv_all(fd, reinterpret_cast<char*>(&net_len), sizeof(net_len)) <= 0)
        return "";

    uint32_t len = ntohl(net_len);
    if (len == 0 || len > max_size)
        return "";

    std::string buf(len, '\0');
    //cannot convert from const _Elem * to char *, buf.data() returns const char*
    if (recv_all(fd, &buf[0], static_cast<int>(len)) <= 0)
        return "";

    return buf;
}