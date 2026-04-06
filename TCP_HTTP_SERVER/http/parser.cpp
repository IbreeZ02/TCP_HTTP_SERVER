#include "parser.h"
#include <sstream>
#include <algorithm>
#include <cctype>

std::string Parser::trim(const std::string& s) {
    size_t start = s.find_first_not_of(" \t\r\n");
    size_t end = s.find_last_not_of(" \t\r\n");
    return (start == std::string::npos) ? "" : s.substr(start, end - start + 1);
}

std::string Parser::to_lower(std::string s) {
    std::transform(s.begin(), s.end(), s.begin(), ::tolower);
    return s;
}

HttpRequest Parser::parse(const std::string& raw) {
    HttpRequest req;

    // 1. Split headers and body
    size_t pos = raw.find("\r\n\r\n");
    if (pos == std::string::npos) return req;
    std::string header_part = raw.substr(0, pos);
    std::string body_part = raw.substr(pos + 4);

    // 2. Request line
    size_t line_end = header_part.find("\r\n");
    std::istringstream rl(header_part.substr(0, line_end));
    rl >> req.method >> req.path >> req.version;

    // 3. Headers
    size_t start = line_end + 2;
    while (start < header_part.size()) {
        size_t end = header_part.find("\r\n", start);
        if (end == std::string::npos) end = header_part.size();
        std::string line = header_part.substr(start, end - start);
        size_t colon = line.find(':');
        if (colon != std::string::npos) {
            std::string key = to_lower(trim(line.substr(0, colon)));
            std::string val = trim(line.substr(colon + 1));
            req.headers[key] = val;
        }
        start = end + 2;
    }

    auto te = req.headers.find("transfer-encoding");
    if (te != req.headers.end() && te->second == "chunked") {
        // reject – return an empty request and let the router handle the error
        req.method = "BAD";  // or set a flag; simplest: throw a custom exception
    }

    // 4. Body
    auto it = req.headers.find("content-length");
    if (it != req.headers.end())
        req.body = body_part.substr(0, std::stoi(it->second));

    return req;
}