#pragma once
#include <iostream>
#include <string>
#include <ctime>
#include "platform.h"

enum ClientState {
	CONNECTED,
	DISCONNECTED
};

struct ClientSocket {
	socket_t socket_descriptor;
	std::string ip;
	int port;
	ClientState state;
};

class Client {
public:
	std::string username;
	time_t join_time;
	ClientSocket socket;

	Client(socket_t fd, const std::string& ip, int port) {
		socket.socket_descriptor = fd;
		socket.ip = ip;
		socket.port = port;
		socket.state = CONNECTED;
		join_time = std::time(nullptr);
	}
};