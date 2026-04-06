#include "buffer_handling.h"

/*std::string read_until_complete(socket_t fd) {
    std::string buffer;
    char chunk[1024];

    while (true) {
        int bytes = ::recv(fd, chunk, sizeof(chunk), 0);  // raw recv, not safe_recv
        if (bytes <= 0) break;
        buffer.append(chunk, bytes);
        if (buffer.find("\r\n\r\n") != std::string::npos)
            break;
    }
    return buffer;
}*/

std::string read_until_complete(socket_t fd) {
    std::string buffer;
    char chunk[4096];

    // 1. Read until end of headers
    while (buffer.find("\r\n\r\n") == std::string::npos) {
        int bytes = ::recv(fd, chunk, sizeof(chunk), 0);
        if (bytes <= 0) return buffer;
        buffer.append(chunk, bytes);
    }

    // 2. Read body based on Content-Length
    size_t header_end = buffer.find("\r\n\r\n") + 4;
    size_t cl_pos = buffer.find("Content-Length: ");
    if (cl_pos != std::string::npos) {
        size_t val_start = cl_pos + 16;
        size_t val_end = buffer.find("\r\n", val_start);
        int content_length = std::stoi(buffer.substr(val_start, val_end - val_start));
        int body_have = (int)buffer.size() - (int)header_end;
        int body_need = content_length - body_have;

        while (body_need > 0) {
            int bytes = (::recv)(fd, chunk, (((body_need) < ((int)sizeof(chunk))) ? (body_need) : ((int)sizeof(chunk))), 0); //min function expanded
            if (bytes <= 0) break;
            buffer.append(chunk, bytes);
            body_need -= bytes;
        }
    }
    return buffer;
}