#include "StdAfx.h"

#include "ClientFTP.h"

ClientFTP::ClientFTP(void)
{
	WSAData wdata;

	if (WSAStartup(MAKEWORD(2, 2), &wdata) != 0)
	{
		std::cerr << "WSAStartup() failed" << std::endl;
	}
	localUsername = "USER ";
	loadLocal();

	handleFnMap[FTPProtocol::CQUIT] = &ClientFTP::CQuit;
	handleFnMap[FTPProtocol::CRETR] = &ClientFTP::CRetr;
	handleFnMap[FTPProtocol::CSTOR] = &ClientFTP::CStor;
	handleFnMap[FTPProtocol::R227_c] = &ClientFTP::RPasv;
	handleFnMap[FTPProtocol::R230_c] = &ClientFTP::RLog;
	handleFnMap[FTPProtocol::R150_c] = &ClientFTP::RStor;
}

ClientFTP::~ClientFTP(void)
{
	WSACleanup();
}

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
	serverAddr.sin_family = remote->h_addrtype;
	serverAddr.sin_port = htons(port);
}

void	ClientFTP::dinitSocket(const char *ip, int port)
{
	HOSTENT	*remote;
	remote = gethostbyname(ip);
	if (remote == NULL)
	{
		std::cerr << "[" << ip << "] unknown host" << std::endl;
		return ;
	}

	dsock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (dsock < 0)
		throw("socket() failed");
	
	memset(&dserverAddr, 0, sizeof(dserverAddr));
	dserverAddr.sin_family = AF_INET;
	memcpy(&dserverAddr.sin_addr, remote->h_addr, remote->h_length);
	dserverAddr.sin_family = remote->h_addrtype;
	dserverAddr.sin_port = htons(port);
}

void	ClientFTP::connect()
{
	if (::connect(sock, (struct sockaddr *) &serverAddr, sizeof(serverAddr)) < 0)
	{
		closesocket(sock);
		throw("connect() failed");
	}
}

void	ClientFTP::dconnect()
{
	if (::connect(dsock, (struct sockaddr *) &dserverAddr, sizeof(dserverAddr)) < 0)
	{
		closesocket(dsock);
		throw("connect() failed");
	}
}

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
		localUsername += acUserName;
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

void	ClientFTP::WSAError(std::string err)
{
	std::cerr << err.c_str() << std::endl;
	std::cerr << "WSA Error Code: " << WSAGetLastError() << std::endl;
}

void	ClientFTP::startMessage()
{
	std::cout << "HELP : [RETR|STOR] [filename path]" << std::endl
		<< "HELP : Type \"QUIT\" to exit" << std::endl << std::endl;
}

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
}

void ClientFTP::CRetr(std::vector<std::string>& Arguments)
{
	if (Arguments.size() < 2)
		return ;
	send(FTPProtocol::CPASV.c_str(), FTPProtocol::CPASV.length());
}

void ClientFTP::CStor(std::vector<std::string>& Arguments)
{
	if (Arguments.size() < 2)
		return ;
	send(FTPProtocol::CPASV.c_str(), FTPProtocol::CPASV.length());
}

void ClientFTP::RPasv(std::vector<std::string>& Arguments)
{
	if (Arguments.size() < 5)
		return ;

	int i[6];

	sscanf((Arguments[4]).c_str(), "(%d,%d,%d,%d,%d,%d)", &i[0], &i[1], &i[2], &i[3], &i[4], &i[5]);
	
	std::stringstream ip;
	ip << i[0] << "." << i[1] << "." << i[2] << "." << i[3];
	int p = i[4] * 256 + i[5];

	std::cout << "LA\n";
	try 
	{
		dinitSocket(ip.str().c_str(), p);
		dconnect();
	}
	catch (char *err)
	{
		WSAError(err);
	}
	std::cout << "ICI\n";

	std::stringstream ss;
	if (lcmd == FTPProtocol::CSTOR)
		ss << FTPProtocol::CSTOR << " " << Arguments[1];
	else if (lcmd == FTPProtocol::CRETR)
		ss << FTPProtocol::CRETR << " " << Arguments[1];
	try
	{
		send(ss.str().c_str(), ss.str().length());
	}
	catch (char *err)
	{
		WSAError(err);
	}
}

void ClientFTP::RStor(std::vector<std::string>& Arguments)
{
	;
}

void ClientFTP::RLog(std::vector<std::string>& Arguments)
{
	std::cout << "User logged" << std::endl;
}

void	ClientFTP::handleCommand()
{
	char cmd[FTPProtocol::CMD_MAX_LENGTH];
	memset(cmd, 0, sizeof(cmd));
	std::cin.getline(cmd, FTPProtocol::CMD_MAX_LENGTH);

	if (cmd[0] == '\0')
	{
		std::cout << localHostname.c_str() << ">";
		return ;
	}
	std::vector<std::string>* Arguments = Parse(cmd);
	if (Arguments->size() == 0)
	{
		std::cout << localHostname.c_str() << ">";
		return ;
	}

	std::map<const std::string, handleFn>::iterator it = handleFnMap.find((*Arguments)[0]);
	if (it != handleFnMap.end())
	{
		((*this).*(handleFnMap[(*it).first]))(*Arguments);
	}
	else
		std::cerr << (*Arguments)[0].c_str() << ": Command not implemented" << std::endl;
	std::cout << localHostname.c_str() << ">";
}

void	ClientFTP::recv(char *cmd)
{
	int	n;

	n = ::recv(sock, cmd, FTPProtocol::CMD_MAX_LENGTH, 0);
	if (n == 0)
		throw("Error : Connection Closed");
	else if (n == SOCKET_ERROR)
		throw("recv() failed");
}

void	ClientFTP::send(const char *str, int length)
{
	int	n;
	n = ::send(sock, str, length, 0);
	if (n == 0)
		throw("Error : Connection Closed");
	else if (n == SOCKET_ERROR)
		throw("send() failed");
}

void	ClientFTP::responseCleanup(char *cmd)
{
	std::string str = cmd;

	while (str.find("\r\n") != std::string::npos)
		str.erase(str.find ("\r\n"), 2);

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
	try 
	{
		send(localUsername.c_str(), localUsername.length());
	}
	catch (char *err)
	{
		std::cerr << err << std::endl;
	}
	while (true)
	{
		handleResponse();
		handleCommand();
	}
}