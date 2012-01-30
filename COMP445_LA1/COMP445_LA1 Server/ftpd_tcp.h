#pragma once

#include <iostream>
#include <WinSock2.h>
#include <WS2tcpip.h>

#pragma comment(lib, "ws2_32.lib")

class ftpd_tcp
{
public:
	ftpd_tcp(void);
	~ftpd_tcp(void);

	void run(void);

private:
	void startMessage(void);
	bool InitSocket(void);
	bool BindSocket(void);
	bool ListenSocket(void);
	bool getLocalHostname(void);

private:
	static const unsigned short LISTEN_PORT = 21;

	int sSocket;
	unsigned short sPort;
	struct sockaddr_in sAddr;
	std::string localHostname;
};
