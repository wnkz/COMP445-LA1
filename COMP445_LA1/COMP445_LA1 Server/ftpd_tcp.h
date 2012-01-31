#pragma once
#pragma comment(lib, "ws2_32.lib")
#include <iostream>
#include <WinSock2.h>
#include <WS2tcpip.h>

#include "TCPThread.h"


class ftpd_tcp
{
public:
	ftpd_tcp(void);
	~ftpd_tcp(void);

	void start(void);

	/*static const unsigned int HOSTNAME_LENGTH = 256;
	static const unsigned int FILENAME_LENGTH = 4096;
	static const unsigned int RESPONSE_LENGTH = 40;
	static const unsigned int BUFFER_LENGTH = 1024;
	static const unsigned int MESSAGE_HEADER_SIZE = 8;*/

	//enum Type
	//{
	//	REQ_SIZE = 1,
	//	REQ_TIME,
	//	RESP
	//};

	//struct Request
	//{
	//	char hostname[HOSTNAME_LENGTH];
	//	char filename[FILENAME_LENGTH];
	//};

	//struct Response
	//{
	//	char response[RESPONSE_LENGTH];
	//};

	//struct Message
	//{
	//	ftpd_tcp::Type type;
	//	int length;
	//	char buffer[BUFFER_LENGTH];
	//};

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
