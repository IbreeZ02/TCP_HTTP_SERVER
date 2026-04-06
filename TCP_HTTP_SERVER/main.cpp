#include "server.h"
//#include <winsock2.h>

int main() {
#ifdef _WIN32
    WSADATA wsa;
    WSAStartup(MAKEWORD(2, 2), &wsa);
#endif
    try {
    //or 127.0.0.1 for one device
        Server server("0.0.0.0", 8080);
        server.start();
    }
    catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    }
#ifdef _WIN32
    WSACleanup();
#endif
    return 0;
}
