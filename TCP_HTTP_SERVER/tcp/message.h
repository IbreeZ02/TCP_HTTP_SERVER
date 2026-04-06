#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <ctime>
#include "client.h"

enum MessageType {
	DIRECT,
	BROADCAST,
	JOIN,
	LEAVE
};

struct Message {
	Client* sender;
	Client* receiver; //broadcasting when the pointer = null
	std::vector<uint8_t> buffer;
	std::string content;
	time_t timestamp;
	MessageType type;
};