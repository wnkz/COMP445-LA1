#include "StdAfx.h"
#include "ftpd_tcp.h"


ftpd_tcp::ftpd_tcp(void)
{
	WSAData wdata;

	if (WSAStartup(MAKEWORD(2, 2), &wdata) != 0) {
		std::cerr << "WSAStartup() failed" << std::endl;
	}

	if (InitSocket()) {
		BindSocket();
		ListenSocket();
	} else {
		std::cerr << "Could not initialize socket" << std::endl;
	}

	if (getLocalHostname()) {
		startMessage();
	} else {
		std::cerr << "Could not get the local hostname" << std::endl;
	}
}


ftpd_tcp::~ftpd_tcp(void)
{
	WSACleanup();
}

void ftpd_tcp::run(void)
{
	int clientSocket;
	int clientLenght;
	struct sockaddr_in clientAddr;

	while (42) {
		clientLenght = sizeof(sAddr);
		if ((clientSocket = accept(sSocket, (struct sockaddr*)&clientAddr, &clientLenght)) < 0) {
			std::cerr << "run(): accept failed" << std::endl;
		}

		//TODO: Handle connection(s)
		std::cout << "accept(): new socket: " << clientSocket << std::endl;
	}
}

void ftpd_tcp::startMessage(void)
{
	std::cout << "ftpd_tcp starting at host: " << this->localHostname.c_str() << std::endl;
	std::cout << "waiting to be contacted for transfering files..." << std::endl;
}

bool ftpd_tcp::InitSocket(void)
{
	if ((sSocket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
		return false;
	}

	sPort = ftpd_tcp::LISTEN_PORT;
	memset(&sAddr, 0, sizeof(sAddr));
	sAddr.sin_family = AF_INET;               
	sAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	sAddr.sin_port = htons(sPort);       

	return true;
}

bool ftpd_tcp::BindSocket(void)
{
	if ((bind(sSocket, (struct sockaddr*)&sAddr, sizeof(sAddr))) < 0) {
		return false;
	}
	return true;
}

bool ftpd_tcp::ListenSocket(void)
{
	if ((listen(sSocket, SOMAXCONN)) < 0) {
		return false;
	}
	return true;
}

bool ftpd_tcp::getLocalHostname(void)
{
	char hname[256];
	int r;

	r = gethostname(hname, sizeof(hname));
	if (r == SOCKET_ERROR)
	{
		//TODO: Error checking.
		localHostname = "";
		return false;
	}
	else
	{
		localHostname = hname;
		return true;
	}
 }