#include "connection.h"

bool Connection::should_keep_alive(const HttpRequest& req) {
    auto conn = req.headers.find("connection");
    return (req.version == "HTTP/1.1") &&
        (conn == req.headers.end() || conn->second != "close");
}

void Connection::apply(const HttpRequest& req, HttpResponse& res) {
    if (should_keep_alive(req))
        res.headers["Connection"] = "keep-alive";
    else
        res.headers["Connection"] = "close";
}