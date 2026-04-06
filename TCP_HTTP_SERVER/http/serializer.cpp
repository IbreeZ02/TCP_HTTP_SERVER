#include "serializer.h"

std::string Serializer::serialize(const HttpResponse& res) {
    std::string out;

    // 1. Status line
    out += res.version + " " + std::to_string(res.status_code) + " " + res.status_text + "\r\n";

    // 2. Headers
    //structured binding, c++17
    for (const auto& [key, val] : res.headers)
        out += key + ": " + val + "\r\n";

    // 3. Content-Length (always derived from actual body size)
    // out += "Content-Length: " + std::to_string(res.body.size()) + "\r\n";
    // Only add Content-Length for non-streaming responses
    if (res.headers.find("Content-Type") == res.headers.end() ||
        res.headers.at("Content-Type") != "text/event-stream") {
        out += "Content-Length: " + std::to_string(res.body.size()) + "\r\n";
    }


    // 4. Blank line
    out += "\r\n";

    // 5. Body
    out += res.body;

    return out;
}