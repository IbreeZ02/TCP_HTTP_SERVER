#include "router.h"
#include "file_handler.h"
#include "chat_handler.h"

HttpResponse Router::handle(const HttpRequest& req, socket_t fd) {
    // GET / or GET /index.html
    if (req.method == "GET" && (req.path == "/" || req.path == "/index.html"))
        return FileHandler::serve("static/index.html");

    // GET /chat
    if (req.method == "GET" && req.path == "/chat")
        return FileHandler::serve("static/chat.html");

    if (req.method == "GET" && req.path == "/file")
        return FileHandler::serve("static/file.html");

	// GET /stream — SSE connection, must stay
	if (req.method == "GET" && req.path == "/stream")
    		return ChatHandler::stream(req, fd);

    // POST /send   browser sends a chat message
    if (req.method == "POST" && req.path == "/send")
        return ChatHandler::receive(req);
       
    // instead of req.path.starts_with("/upload/")
    if (req.method == "POST" && req.path.find("/upload/") == 0)
        return FileHandler::upload(req);

    if (req.method == "GET" && req.path.find("/download/") == 0)
        return FileHandler::download(req.path);

    HttpResponse res;
    if (req.method == "BAD") {
        res.status_code = 400;
        res.status_text = "Bad Request";
        res.body = "Chunked encoding not supported";
        return res;
    }
   
    // 404 fallback
    res.status_code = 404;
    res.status_text = "Not Found";
    res.body = "404 Not Found";
    return res;
}
