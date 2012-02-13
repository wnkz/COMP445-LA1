#pragma once
#pragma comment(lib, "ws2_32.lib")
#include <iostream>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <exception>

#include "FTPdThread.h"

class FTPd
{
public:
	FTPd(void);
	~FTPd(void);

	void start(void);

private:
	void startMessage(void);
	void InitSocket(void);
	void BindSocket(void);
	void ListenSocket(void);
	void LoadLocalHostname(void);
	void GetLocalIP(void);

	void RuntimeError(std::string serr);
	void WSAError(std::string serr);

private:
	SOCKET so;
	unsigned short port;
	SOCKADDR_IN sAddr;

	std::string localIP;
	std::string localHostname;

private:
	const static unsigned short DEFAULT_LISTEN_PORT = 21;
};

