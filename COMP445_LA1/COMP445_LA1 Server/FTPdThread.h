#pragma once
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

#include "BaseThread.h"
#include "FTPProtocol.h"

class FTPdThread :
	public BaseThread
{
public:
	FTPdThread(std::string IP, SOCKET Socket, SOCKADDR_IN* Addr);
	~FTPdThread(void);

	void run(void);

private:
	void CommandCleanup(char* cmd);
	std::vector<std::string>* CommandParse(char* cmd);
	void handleCommand(char* cmd);

	bool CheckUserSession(void);
	void CUser(std::vector<std::string>& Arguments);
	void CList(std::vector<std::string>& Arguments);
	void CSyst(std::vector<std::string>& Arguments);
	void CPasv(std::vector<std::string>& Arguments);
	void CPwd(std::vector<std::string>& Arguments);
	void CRetr(std::vector<std::string>& Arguments);
	void CStor(std::vector<std::string>& Arguments);
	void CQuit(std::vector<std::string>& Arguments);

	typedef void (FTPdThread::*handleFn)(std::vector<std::string>& Arguments);

	std::map<const std::string, handleFn> handleFnMap;

private:
	char* ClientIP(void) const;
	unsigned short ClientPort(void) const;

	static unsigned short GetNextDataPort(void);

private:
	std::string ip, ipcomma;

	SOCKET s;
	SOCKADDR_IN addr;

	SOCKET ds;
	unsigned short dport;

	std::string userLogin;
	bool userSession;

	static const unsigned int STORBUFFERSIZE = 4096;
	static const std::string FILE_DIRECTORY;
};
