#pragma once

#include <winsock2.h>

#define SERVER_PORT 21

class TcpClient {
public:
	TcpClient();
	~TcpClient();

	void			run();

	void			sendFile();
	void			receiveFile();

	void			fillMsg();
	void			sendMsg();
	void			receiveMsg();

protected:
private:
	int					sock;
	struct sockaddr_in	ServerAddr;
	WSADATA				wsadata;
};