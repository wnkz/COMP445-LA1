#include "StdAfx.h"
#include "FTPdThread.h"


FTPdThread::FTPdThread(std::string IP, SOCKET Socket, SOCKADDR_IN* Addr) : ip(IP), ipcomma(IP), s(Socket), addr(*Addr)
{
	std::replace(ipcomma.begin(), ipcomma.end(), '.', ',');

	handleFnMap[FTPProtocol::CUSER] = &FTPdThread::CUser;
	/* TODO: Implement if needed
	handleFnMap[FTPProtocol::CLIST] = &FTPdThread::CList; */
	handleFnMap[FTPProtocol::CSYST] = &FTPdThread::CSyst;
	handleFnMap[FTPProtocol::CPASV] = &FTPdThread::CPasv;
	handleFnMap[FTPProtocol::CPWD] = &FTPdThread::CPwd;
	handleFnMap[FTPProtocol::CRETR] = &FTPdThread::CRetr;

	userLogin = "";
	userSession = false;
}


FTPdThread::~FTPdThread(void)
{
}

void FTPdThread::run(void)
{
	char cmd[FTPProtocol::CMD_MAX_LENGTH];
	int n;

	send(s, FTPProtocol::R220.c_str(), FTPProtocol::R220.length(), 0);

	memset(cmd, 0, sizeof(cmd));
	while (true)
	{
		n = recv(s, cmd, FTPProtocol::CMD_MAX_LENGTH, 0);
		if (n == 0) {
			std::cerr << "CONNECTION CLOSED (" << ClientIP() << ":" << ClientPort() << ")" << std::endl;
			break ;
		}
		else if (n == SOCKET_ERROR) {
			std::cerr << "WSA Error Code:" << WSAGetLastError() << std::endl;
			//WSACleanup();
			break ;
		}
		else {
			handleCommand(cmd);
		}
		memset(cmd, 0, sizeof(cmd));
	}
}

char* FTPdThread::ClientIP(void) const
{
	return inet_ntoa(addr.sin_addr);
}

unsigned short FTPdThread::ClientPort(void) const
{
	return ntohs(addr.sin_port);
}

unsigned short FTPdThread::GetNextDataPort(void)
{
	static unsigned short port = 4000;

	port++;
	return (port - 1);
}

bool FTPdThread::CheckUserSession(void)
{
	if (!userSession) {
		send(s, FTPProtocol::R530.c_str(), FTPProtocol::R530.length(), 0);
		return false;
	}
	return true;
}

#pragma region Commands

void FTPdThread::handleCommand(char* cmd)
{
	CommandCleanup(cmd);
	std::vector<std::string>* Arguments = CommandParse(cmd);

	std::cout << "\tCOMMAND: (" << cmd << ")" << std::endl;

	if (Arguments->size() == 0)
		return ;

	std::map<const std::string, handleFn>::iterator it = handleFnMap.find((*Arguments)[0]);
	if (it != handleFnMap.end()) {
		((*this).*(handleFnMap[(*it).first]))(*Arguments);
	} else {
		send(s, FTPProtocol::R502.c_str(), FTPProtocol::R502.length(), 0);
	}
}

void FTPdThread::CommandCleanup(char* cmd)
{
	std::string str = cmd;
	while (str.find("\r\n") != std::string::npos)
	{
		str.erase(str.find ("\r\n"), 2);
	}

	strcpy(cmd, str.c_str());
}

std::vector<std::string>* FTPdThread::CommandParse(char* cmd)
{
	std::vector<std::string>* v = new std::vector<std::string>();
	std::istringstream iss(cmd);

	std::copy(	std::istream_iterator<std::string>(iss),
				std::istream_iterator<std::string>(),
				std::back_inserter<std::vector<std::string>>(*v));
	return v;
}

void FTPdThread::CUser(std::vector<std::string>& Arguments)
{
	userLogin = "";
	userSession = false;

	if (Arguments.size() <= 1) {
		send(s, FTPProtocol::R501.c_str(), FTPProtocol::R501.length(), 0);
		return ;
	}

	userLogin = Arguments[1];
	userSession = true;
	send(s, FTPProtocol::R230.c_str(), FTPProtocol::R230.length(), 0);
}

void FTPdThread::CList(std::vector<std::string>& Arguments)
{
	
}

void FTPdThread::CSyst(std::vector<std::string>& Arguments)
{
	send(s, FTPProtocol::R215.c_str(), FTPProtocol::R215.length(), 0); 
}

void FTPdThread::CPasv(std::vector<std::string>& Arguments)
{
	std::stringstream ss;
	dport = GetNextDataPort();

	ss << FTPProtocol::R227	<< " ("
		<< ipcomma << ","
		<< dport / 256 << "," << dport % 256
		<< ")." << std::endl;

	SOCKADDR_IN	sin, nsin;
	PROTOENT* pe;
	socklen_t len;
	SOCKET so;

	pe = getprotobyname("tcp");
	so = socket(PF_INET, SOCK_STREAM, pe->p_proto);
	sin.sin_family = AF_INET;
	sin.sin_port = htons(dport);
	sin.sin_addr.s_addr = INADDR_ANY;
	bind(so, (SOCKADDR*)&(sin), sizeof(sin));
	listen(so, 1);
	len = sizeof(nsin);

	send(s, ss.str().c_str(), ss.str().length(), 0);
	ds = accept(so, (SOCKADDR*)&(nsin), &(len));
	closesocket(so);
}

void FTPdThread::CPwd(std::vector<std::string>& Arguments)
{
	// Dummy PWD
	send(s, "257 \"/\"\n", 8, 0);
}

void FTPdThread::CRetr(std::vector<std::string>& Arguments)
{
	std::ifstream::pos_type size;
	char* memblock;
	char currentPath[FILENAME_MAX];
	std::string filePath;

	_getcwd(currentPath, sizeof(currentPath) / sizeof(TCHAR));
	filePath = currentPath + std::string("\\") + Arguments[1];

	std::ifstream file(filePath, std::ios::in|std::ios::binary|std::ios::ate);
	if (file.is_open())
	{
		size = file.tellg();
		memblock = new char[size];
		file.seekg(0, std::ios::beg);
		file.read(memblock, size);
		file.close();

		std::stringstream ss;

		ss << FTPProtocol::R150_RETR << Arguments[1] << "(" << size << ")." << std::endl;
		send(s, ss.str().c_str(), ss.str().length(), 0);

		send(ds, memblock, size, 0);
		closesocket(ds);

		send(s, FTPProtocol::R226_RETR.c_str(), FTPProtocol::R226_RETR.length(), 0);

		delete[] memblock;
	} else {
		send(s, FTPProtocol::R550.c_str(), FTPProtocol::R550.length(), 0);
	}
}

#pragma endregion