#pragma once
#include <iostream>
#include <string>
#include <list>
#include "client.h"
#include <mutex>
#include "message.h"
#include "platform.h"
#include <atomic>
#include <thread>

enum ServerState {
	IDLE,
	RUNNING,
	CLOSED
};

struct ServerSocket{
	socket_t socket_descriptor;
	std::string ip;
	int port;
	ServerState state;
};

class Server {
public:
	//constrcutor
	Server(std::string ip, int port) {
		socket.ip = ip;
		socket.port = port;
		socket.socket_descriptor = -1;  //not created yet
		socket.state = IDLE;
		//running = false;
	};
	
	ServerSocket socket;
	//better store clients or clinets sockets than just their IDs
		//and even better user client since it holds it's socket (& it's a pointer obvi)
	std::list<Client*> connected_clients;
	std::mutex clients_mutex;

	void start();
	
	void stop();

	//*used inside other functions
	void bind(); //bind socket to ip and port
	void listen(); //queue incoming connections: active until server is closed 
	void accept(); //pulls connections from lisent queue
	void connect(Client* Client);
	void disconnect(Client* Client);

	//handles program loop
	void handle_client(Client* client);
	/* only a server can broadcast:
	* 1. a client konws itself
	* 2. a client doesn't have full list of all connected clients */

	//void broadcast(Message& message); using websocket
	void broadcast(const std::string& message); //using SSE

private:
	std::atomic<bool> running{ false };
};


/* HOW handle_client loop works :
1. reading raw bytes, if (raw.empty()) break;
2. parse HttpRequest req = parse.parse(raw);
3. get request and pass to router
	HttpResponse res = router.handle(req);
4. from router to app layer
5. building response in app layer
6. serializaing to bytes and send to client by safe_send
	safe_send(client->socket.socket_descriptor, res.serialize());
7. check keep-alive or close
	if keep-alive loop back and wait for next request (nothing to code)
	if (!req.keep_alive) break; -> close socket/thread ends --> built-in
8. if SSE uses binary length framing, browsers get garbage * *
	SSE::send_event → safe_send → send_message →[4 - byte binary length] + [data]
*/