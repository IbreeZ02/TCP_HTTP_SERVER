#pragma once
#include "platform.h"
#include <string>

class SSE {
public:
    static void send_event(socket_t fd, const std::string& data);
};