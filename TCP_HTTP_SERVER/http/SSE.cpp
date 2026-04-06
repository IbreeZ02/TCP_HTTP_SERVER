#include "SSE.h"
#include "network.h"

void SSE::send_event(socket_t fd, const std::string& data) {
    safe_send(fd, "data: " + data + "\n\n");
}

/*The SSE* response* (opening the stream) happens in `chat_handler`
when the browser hits `/chat / stream`, the handler returns the headers and keeps the socket open.
`SSE::send_event` is then called whenever there's a message to push.*/