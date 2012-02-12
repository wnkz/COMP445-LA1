#pragma once
#pragma comment(lib, "ws2_32.lib")

#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <iterator>
#include <vector>
#include <map>
#include <exception>
#include <cstdio>
#include <direct.h>

#include <WinSock2.h>
#include <WS2tcpip.h>

#include "FTPProtocol.h"

class ClientFTP
{
public:
	ClientFTP(void);
	~ClientFTP(void);

	void	initSocket(char *remoteHostName);
	void	initSocketData(const char *ip, unsigned short int port);
	void	connect();
	void	connectData();

	void	loadLocal();
	void	loadLocalUsername();
	void	loadLocalHostname();

	void	startMessage();

	void	recv(char *cmd);
	void	send(const char *str, int length);
	int		recvData(char *cmd);
	void	sendData(const char *str, int length);

	void	responseCleanup(char* cmd);
	std::vector<std::string>*	Parse(char* cmd);

	void	handleCommand();
	void	handleResponse();

	void	run();

	void	WSAError(std::string err);

private:
	void CQuit(std::vector<std::string>& Arguments);
	void CUser(std::vector<std::string>& Arguments);
	void CRetr(std::vector<std::string>& Arguments);
	void CStor(std::vector<std::string>& Arguments);

	void RPasv(std::vector<std::string>& Arguments);
	void RetrStor(std::vector<std::string>& Arguments);
	void StorRetr(std::vector<std::string>& Arguments);
	void RQuit(std::vector<std::string>& Arguments);
	void RLog(std::vector<std::string>& Arguments);
	void ROpenFailed(std::vector<std::string>& Arguments);
	void RAborted(std::vector<std::string>& Arguments);

	typedef void (ClientFTP::*handleFn)(std::vector<std::string>& Arguments);

	std::map<const std::string, handleFn> handleFnMap;

private:
	SOCKET	sock;
	SOCKET  sockData;
	
	unsigned short	port;

	SOCKADDR_IN	serverAddr;
	SOCKADDR_IN serverAddrData;

	std::string localHostname;
	std::string	localUsername;
	std::string filename;
	std::string lcmd;

	bool	first;

private:
	const static unsigned short DEFAULT_LISTEN_PORT = 21;
	static const unsigned int STORBUFFERSIZE = 4096;
};
