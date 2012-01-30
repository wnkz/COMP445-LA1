#include "StdAfx.h"
#pragma comment(lib,"wsock32.lib")

#include <iostream>

#include "client_tcp.h"

TcpClient::TcpClient() {
}

TcpClient::~TcpClient() {
}

void	TcpClient::sendFile() {

}

void	TcpClient::receiveFile() {

}

void	TcpClient::sendMsg() {

}

void	TcpClient::receiveMsg() {

}

void	TcpClient::run() {
	try {
		if (WSAStartup(0x0202, &wsadata) != 0) {  
			WSACleanup();  
			throw("Critical Error : WSAStartup\n");
		}
		char	username[40];
		char	hostname[100];
		unsigned long dword = 40;

		if (!GetUserNameW((LPWSTR)username, &dword))
			throw("Critical Error : GetUserName\n");
		if (gethostname(hostname, 100) != 0) 
			throw("Critical Error : GetHostname\n");
	}
	catch (char* str) {
		std::cerr << str << std::endl;
	}
	while (1) {
		std::cout << ">";
	}
}

unsigned long	TcpClient::resolveName(char name[]) {
	struct hostent *host;
	if ((host = gethostbyname(name)) == NULL)
		throw("Critical Error : ResolveName");
	return *((unsigned long *)host->h_addr_list[0]);
}