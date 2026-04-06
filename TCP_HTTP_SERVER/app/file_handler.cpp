#include "file_handler.h"
#include <fstream>
#include <sstream>
#include "logging.h"
#include "chat_handler.h"


HttpResponse FileHandler::serve(const std::string& path) {
    HttpResponse res;
    std::ifstream file(path);
    if (!file.is_open()) {
        log_error("Could not open file: " + path);
        res.status_code = 404;
        res.status_text = "Not Found";
        res.body = "File not found";
        return res;
    }
    std::stringstream ss;
    ss << file.rdbuf();
    res.body = ss.str();
    /*res.headers["Content-Type"] = path.ends_with(".html") ? "text/html" :
        path.ends_with(".css") ? "text/css" :
        path.ends_with(".js") ? "application/javascript" :
		"text/plain";*/ //ends_with is C++20, not supported in all compilers yet
    res.headers["Content-Type"] =
        path.rfind(".html") == path.size() - 5 ? "text/html" :
        path.rfind(".css") == path.size() - 4 ? "text/css" :
        path.rfind(".js") == path.size() - 3 ? "application/javascript" :
        "text/plain";
    return res;
}

HttpResponse FileHandler::upload(const HttpRequest& req) {
    HttpResponse res;

    // extract filename from /upload/<filename>
    std::string filename = req.path.substr(8); // skip "/upload/"

    if (filename.empty() || req.body.empty()) {
        res.status_code = 400;
        res.status_text = "Bad Request";
        res.body = "Bad request";
        return res;
    }

    std::ofstream out("uploads/" + filename, std::ios::binary);
    if (!out) {
        res.status_code = 500;
        res.status_text = "Internal Server Error";
        res.body = "Could not save file";
        return res;
    }

    out.write(req.body.data(), req.body.size());
    ChatHandler::broadcast("file: " + filename + ":" + std::to_string(req.body.size()));
    res.body = "Uploaded: " + filename;
    return res;
}

static std::string url_decode(const std::string& s) {
    std::string result;
    for (size_t i = 0; i < s.size(); ++i) {
        if (s[i] == '%' && i + 2 < s.size()) {
            int val = std::stoi(s.substr(i + 1, 2), nullptr, 16);
            result += static_cast<char>(val);
            i += 2;
        } else if (s[i] == '+') {
            result += ' ';
        } else {
            result += s[i];
        }
    }
    return result;
}


HttpResponse FileHandler::download(const std::string& path) {
    HttpResponse res;
	std::string filename = url_decode(path.substr(10));
    // extract filename from /download/<filename>
    //std::string filename = path.substr(10); // skip "/download/"
    std::ifstream file("uploads/" + filename, std::ios::binary);
    if (!file) {
        res.status_code = 404;
        res.status_text = "Not Found";
        res.body = "File not found";
        return res;
    }

    std::ostringstream ss;
    ss << file.rdbuf();
    res.body = ss.str();
    res.headers["Content-Disposition"] = "attachment; filename=\"" + filename + "\"";
    res.headers["Content-Type"] = "application/octet-stream";
    return res;
}
