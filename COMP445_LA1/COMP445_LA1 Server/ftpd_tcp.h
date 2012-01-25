#pragma once

#include <iostream>
#include <winsock2.h>
#include <WS2tcpip.h>

#pragma comment(lib, "ws2_32.lib")

class ftpd_tcp
{
public:
	ftpd_tcp(void);
	~ftpd_tcp(void);

	void startMessage(void);
	std::string getHostname(void);
};
