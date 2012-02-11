#include "StdAfx.h"
#include "FTPd.h"

const std::string FTPd::IP = "172.16.110.133";

FTPd::FTPd(void)
{
	WSAData wdata;

	if (WSAStartup(MAKEWORD(2, 2), &wdata) != 0) {
		std::cerr << "WSAStartup() failed" << std::endl;
	}

	InitSocket();
	BindSocket();
	ListenSocket();

	LoadLocalHostname();
}

FTPd::~FTPd(void)
{
	WSACleanup();
}

void FTPd::start(void)
{
	SOCKET c;
	socklen_t cLenght;
	struct sockaddr_in cAddr;

	startMessage();

	while (42) {
		cLenght = sizeof(sAddr);
		if ((c = accept(so, (struct sockaddr*)&cAddr, &cLenght)) < 0) {
			WSAError("accept() failed");
		}

		std::cout << "CONNECTION: " << inet_ntoa(cAddr.sin_addr) << ":" << ntohs(cAddr.sin_port) << std::endl;
		FTPdThread* cThread = new FTPdThread(IP, c, &cAddr);
		cThread->start();
	}
}

void FTPd::startMessage(void)
{
	std::cout << "ftpd_tcp starting at host: " << localHostname.c_str() << std::endl;
	std::cout << "waiting to be contacted for transfering files..." << std::endl;
}

void FTPd::InitSocket(void)
{
	if ((so = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
		WSAError("socket() failed");
	}

	port = FTPd::DEFAULT_LISTEN_PORT;

	memset(&sAddr, 0, sizeof(sAddr));
	sAddr.sin_family = AF_INET;               
	sAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	sAddr.sin_port = htons(port);       
}

void FTPd::BindSocket(void)
{
	if ((bind(so, (struct sockaddr*)&sAddr, sizeof(sAddr))) < 0) {
		WSAError("bind() failed");
	}
}

void FTPd::ListenSocket(void)
{
	if ((listen(so, SOMAXCONN)) < 0) {
		WSAError("listen(): failed");
	}
}

void FTPd::LoadLocalHostname(void)
{
	char hname[256];
	int r;

	r = gethostname(hname, sizeof(hname));
	if (r == SOCKET_ERROR)
	{
		localHostname = "";
	}
	else
	{
		localHostname = hname;
	}
 }

void FTPd::WSAError(std::string serr)
{
	std::cerr << serr.c_str() << std::endl;
	std::cerr << "WSA Error Code:" << WSAGetLastError() << std::endl;
	WSACleanup();
}

void FTPd::RuntimeError(std::string serr)
{
	std::cerr << serr.c_str() << std::endl;
	throw std::runtime_error(serr);
}