#pragma once

#include <winsock2.h>

class TcpClient {
public:
	TcpClient();
	~TcpClient();

	void			run();

	void			sendFile();
	void			receiveFile();

	void			sendMsg();
	void			receiveMsg();

	unsigned long	resolveName(char name[]);

protected:
private:
	int					sock;
	struct sockaddr_in	ServerAddr;
	unsigned short		ServerPort;
	WSADATA				wsadata;
};