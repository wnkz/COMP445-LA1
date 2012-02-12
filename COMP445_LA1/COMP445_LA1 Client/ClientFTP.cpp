#include "StdAfx.h"

#include "ClientFTP.h"

ClientFTP::ClientFTP(void)
{
	WSAData wdata;

	if (WSAStartup(MAKEWORD(2, 2), &wdata) != 0)
	{
		std::cerr << "WSAStartup() failed" << std::endl;
	}
	loadLocal();

	handleFnMap[FTPProtocol::CQUIT] = &ClientFTP::CQuit;
	handleFnMap[FTPProtocol::CUSER] = &ClientFTP::CUser;
	handleFnMap[FTPProtocol::CRETR] = &ClientFTP::CRetr;
	handleFnMap[FTPProtocol::CSTOR] = &ClientFTP::CStor;
	handleFnMap[FTPProtocol::R227_c] = &ClientFTP::RPasv;
	handleFnMap[FTPProtocol::R230_c] = &ClientFTP::RLog;
	handleFnMap[FTPProtocol::R221_c] = &ClientFTP::RQuit;
	handleFnMap[FTPProtocol::R226_c] = &ClientFTP::StorRetr;
	handleFnMap[FTPProtocol::R150_c] = &ClientFTP::RetrStor;
	handleFnMap[FTPProtocol::R550_c] = &ClientFTP::ROpenFailed;
	handleFnMap[FTPProtocol::R451_c] = &ClientFTP::RAborted;

	first = true;
}

ClientFTP::~ClientFTP(void)
{
	WSACleanup();
}

#pragma region init&connect
void	ClientFTP::initSocket(char *remoteHostname)
{
	HOSTENT	*remote;
	remote = gethostbyname(remoteHostname);
	if (remote == NULL)
	{
		std::cerr << "[" << remoteHostname << "] unknown host" << std::endl;
		return ;
	}

	sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sock < 0)
		throw("socket() failed");

	port = ClientFTP::DEFAULT_LISTEN_PORT;
	
	memset(&serverAddr, 0, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	memcpy(&serverAddr.sin_addr, remote->h_addr, remote->h_length);
	serverAddr.sin_port = htons(port);
}

void	ClientFTP::initSocketData(const char *ip, unsigned short int portData)
{
	port = portData;
	sockData = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sockData < 0)
		throw("socket() failed");
	
	memset(&serverAddrData, 0, sizeof(serverAddrData));
	serverAddrData.sin_family = AF_INET;
	serverAddrData.sin_addr.s_addr = inet_addr(ip);
	serverAddrData.sin_port = htons(port);
}

void	ClientFTP::connect()
{
	if (::connect(sock, (struct sockaddr *) &serverAddr, sizeof(serverAddr)) < 0)
	{
		closesocket(sock);
		throw("connect() failed");
	}
}

void	ClientFTP::connectData()
{
	if (::connect(sockData, (struct sockaddr *) &serverAddrData, sizeof(serverAddrData)) < 0)
	{
		closesocket(sockData);
		throw("connectData() failed");
	}
}

#pragma endregion

#pragma region local
void	ClientFTP::loadLocal()
{
	loadLocalUsername();
	loadLocalHostname();
}

void	ClientFTP::loadLocalUsername()
{
	char acUserName[256];

	DWORD nUserName = 256;
	if (GetUserName((LPWSTR)acUserName, &nUserName))
		localUsername = acUserName;
}

void	ClientFTP::loadLocalHostname()
{
	char	hname[256];
	int		r;

	r = gethostname(hname, sizeof(hname));
	if (r == SOCKET_ERROR)
		localHostname = "";
	else
		localHostname = hname;
}
#pragma endregion

#pragma region help&error
void	ClientFTP::WSAError(std::string err)
{
	std::cerr << err.c_str() << std::endl;
	std::cerr << "WSA Error Code: " << WSAGetLastError() << std::endl;
}

void	ClientFTP::startMessage()
{
	std::cout << "HELP : [RETR|STOR] [filename]" << std::endl
		<< "HELP : Type \"QUIT\" to exit" << std::endl << std::endl;
}
#pragma endregion


void ClientFTP::CQuit(std::vector<std::string>& Arguments)
{
	try 
	{
		send(FTPProtocol::CQUIT.c_str(), FTPProtocol::CQUIT.length());
	}
	catch (char *err)
	{
		WSAError(err);
	}
	handleResponse();
}

void ClientFTP::CUser(std::vector<std::string>& Arguments)
{
	try
	{
	std::stringstream ss;
	ss << FTPProtocol::CUSER.c_str() << " " << localUsername.c_str();
	send(ss.str().c_str(), ss.str().length());
	}
	catch (char *err)
	{
		WSAError(err);
	}
	handleResponse();
}

void ClientFTP::CRetr(std::vector<std::string>& Arguments)
{
	if (Arguments.size() < 2)
		return ;
	send(FTPProtocol::CPASV.c_str(), FTPProtocol::CPASV.length());
	lcmd = FTPProtocol::CRETR;
	filename = Arguments[1];
	handleResponse();
}

void ClientFTP::CStor(std::vector<std::string>& Arguments)
{
	if (Arguments.size() < 2)
		return ;
	send(FTPProtocol::CPASV.c_str(), FTPProtocol::CPASV.length());
	lcmd = FTPProtocol::CSTOR;
	filename = Arguments[1];
	handleResponse();
}

void ClientFTP::RPasv(std::vector<std::string>& Arguments)
{
	if (Arguments.size() < 5)
		return ;

	int i[6];

	sscanf((Arguments[4]).c_str(), "(%d,%d,%d,%d,%d,%d)", &i[0], &i[1], &i[2], &i[3], &i[4], &i[5]);
	
	std::stringstream ip;
	ip << i[0] << "." << i[1] << "." << i[2] << "." << i[3];
	unsigned short int p = i[4] * 256 + i[5];

	try 
	{
		initSocketData(ip.str().c_str(), p);
		connectData();
	}
	catch (char *err)
	{
		WSAError(err);
		return ;
	}

	std::stringstream ss;
	if (lcmd == FTPProtocol::CSTOR)
		ss << FTPProtocol::CSTOR.c_str() << " " << filename.c_str();
	else if (lcmd == FTPProtocol::CRETR)
		ss << FTPProtocol::CRETR.c_str() << " " << filename.c_str();
	try
	{
		send(ss.str().c_str(), ss.str().length());
	}
	catch (char *err)
	{
		WSAError(err);
	}
	handleResponse();
}

void ClientFTP::ROpenFailed(std::vector<std::string>& Arguments)
{
	std::cerr << "Failed to open \"" << filename.c_str() << "\"" <<std::endl;
	if (sockData != NULL)
		closesocket(sockData);
	filename.clear();
	lcmd.clear();
}

void ClientFTP::RAborted(std::vector<std::string>& Arguments)
{
	std::cerr << "Requested action aborted" << std::endl;
	if (sockData != NULL)
		closesocket(sockData);
	filename.clear();
	lcmd.clear();
}

void ClientFTP::RetrStor(std::vector<std::string>& Arguments)
{
	if (lcmd == FTPProtocol::CSTOR)
	{
		std::ifstream::pos_type size;
		char* memblock;

		std::ifstream file(filename, std::ios::in|std::ios::binary|std::ios::ate);
		if (file.is_open())
		{
			size = file.tellg();
			memblock = new char[size];
			file.seekg(0, std::ios::beg);
			file.read(memblock, size);
			file.close();
			sendData(memblock, size);
			delete[] memblock;
			closesocket(sockData);
			handleResponse();
		}
	}
	else if (lcmd == FTPProtocol::CRETR)
	{
		char memblock[ClientFTP::STORBUFFERSIZE];
		memset(memblock, 0, ClientFTP::STORBUFFERSIZE);
		std::ofstream file(filename, std::ios::out|std::ios::binary|std::ios::trunc);
		if (file.is_open())
		{
			int r;
			while ((r = recvData(memblock)) != 0)
			{
				if (r == SOCKET_ERROR)
				{
					WSAError("recvData() failed");
					file.close();
					closesocket(sockData);
					break ;
				}
				file << memblock;
			}
		file.close();
		}
		handleResponse();
	}
}

void ClientFTP::StorRetr(std::vector<std::string>& Arguments)
{
	if (lcmd == FTPProtocol::CRETR)
	{
		if (sockData != NULL)
			closesocket(sockData);
	}
	filename.clear();
	lcmd.clear();
}

void ClientFTP::RQuit(std::vector<std::string>& Arguments)
{
	if (sock != NULL)
		closesocket(sock);
	exit(0);
}

void ClientFTP::RLog(std::vector<std::string>& Arguments)
{
	std::cout << "User logged" << std::endl;
}

void	ClientFTP::handleCommand()
{
	char cmd[FTPProtocol::CMD_MAX_LENGTH];
	memset(cmd, 0, sizeof(cmd));
	if (first == true)
	{
		handleResponse();
		first = false;
		std::stringstream ss;
		ss << FTPProtocol::CUSER.c_str();
		ss >> cmd;
	}
	else
		std::cin.getline(cmd, FTPProtocol::CMD_MAX_LENGTH);

	if (cmd[0] == '\0')
		return ;
	std::vector<std::string>* Arguments = Parse(cmd);
	if (Arguments->size() == 0)
		return ;

	std::map<const std::string, handleFn>::iterator it = handleFnMap.find((*Arguments)[0]);
	if (it != handleFnMap.end())
	{
		((*this).*(handleFnMap[(*it).first]))(*Arguments);
	}
	else
		std::cerr << (*Arguments)[0].c_str() << ": Command not implemented" << std::endl;
}

#pragma region send&recv

void	ClientFTP::recv(char *cmd)
{
	int	n;

	n = ::recv(sock, cmd, FTPProtocol::CMD_MAX_LENGTH, 0);
	if (n == 0)
		throw("Error : recv() : Connection Closed");
	else if (n == SOCKET_ERROR)
		throw("recv() failed");
}

int ClientFTP::recvData(char *cmd)
{
	int	n;

	n = ::recv(sockData, cmd, ClientFTP::STORBUFFERSIZE, 0);
	return n;
}

void	ClientFTP::send(const char *str, int length)
{
	int	n;
	n = ::send(sock, str, length, 0);
	if (n == 0)
		throw("Error : send() : Connection Closed");
	else if (n == SOCKET_ERROR)
		throw("send() failed");
}

void	ClientFTP::sendData(const char *str, int length)
{
	int	n;

	n = ::send(sockData, str, length, 0);
	if (n == 0)
		throw("Error : send() : Connection Closed");
	else if (n == SOCKET_ERROR)
		throw("send() failed");
}

#pragma endregion

void	ClientFTP::responseCleanup(char *cmd)
{
	std::string str = cmd;

	while (str.find("\r\n") != std::string::npos)
		str.erase(str.find ("\r\n"), 2);
	memset(cmd, 0, sizeof(cmd));
	strncpy(cmd, str.c_str(), str.length());
}

std::vector<std::string>* ClientFTP::Parse(char* cmd)
{
	std::vector<std::string>* v = new std::vector<std::string>();
	std::istringstream iss(cmd);

	std::copy(std::istream_iterator<std::string>(iss),
		std::istream_iterator<std::string>(),
		std::back_inserter<std::vector<std::string>>(*v));
	return v;
}

void	ClientFTP::handleResponse()
{
	char cmd[FTPProtocol::CMD_MAX_LENGTH];
	memset(cmd, 0, sizeof(cmd));
	try
	{
		recv(cmd);
	}
	catch (char *err)
	{
		WSAError(err);
		return ;
	}
	if (cmd[0] == '\0')
		return ;
	responseCleanup(cmd);

	std::vector<std::string>* Arguments = Parse(cmd);
	if (Arguments->size() == 0)
		return ;
	std::map<const std::string, handleFn>::iterator it = handleFnMap.find((*Arguments)[0]);
	if (it != handleFnMap.end())
		((*this).*(handleFnMap[(*it).first]))(*Arguments);
}

void	ClientFTP::run()
{
	char	remoteHostname[256];
	std::cout << "Enter Server name: ";
	std::cin >> remoteHostname;
	try 
	{
		initSocket(remoteHostname);
		connect();
	}
	catch (char *err)
	{
		WSAError(err);
		return ;
	}
	startMessage();
	while (true)
	{
		if (first != true)
			std::cout << localHostname.c_str() << ">";
		handleCommand();
	}
}