#include "StdAfx.h"
#pragma comment(lib,"wsock32.lib")

#include <iostream>

#include "client_tcp.h"

TcpClient::TcpClient() {
}

TcpClient::~TcpClient() {
}

void	TcpClient::sendFile(/*char* filename*/) {
	/*Request *req
	bool first = true;

	FILE * stream = fopen(filename, "r+b");

	if (stream == NULL)
		return ;
	while (!feof(stream))
     {
       if (fgets(buffer , BUFFER_LENGTH , stream) != NULL) {
		   /!\Fill Msg(first ? INITIAL_DATA : DATA)
	   }
	   sendMsg(req);
	   first = false;
     }
	 sendMsg for finish
     fclose (stream);
	 */
}

void	TcpClient::receiveFile() {

}

void	TcpClient::fillMsg() {

}

void	TcpClient::sendMsg(/*Resquest req*/) {
	/*
	int size;
	size = send(sock,(char *)req, req.size, 0))
	if (size == -1)
		throw("Critical Error : send\n");
	*/
}

void	TcpClient::receiveMsg() {
	/*
	int	size;
	char buf[MAX_REQUEST_SIZE];
	t_header header;

	void *body;
	Response *response;

	size = ::recv(sock, buf, sizeof(buf), 0);
	if (size == -1 || size < sizeof(header))
		throw("Critical Error : recv\n");
	memcpy(&header, buf, sizeof(header));

	body = reinterpret_cast<void *>(new char[size - sizeof(header)]);
	memcpy(body, &buf[sizeof(header)], size - sizeof(header));
	response.code = ntohl(header.code);
	response.body = body;
	delete body;
	return reponse;
	*/
}

void	TcpClient::run() {
	std::string	local;

	try {
		if (WSAStartup(0x0202, &wsadata) != 0) {  
			WSACleanup();  
			throw("Critical Error : WSAStartup\n");
		}
		char	username[40];
		char	localhost[100];
		unsigned long dword = 39;

		if (!GetUserNameW((LPWSTR)username, &dword))
			throw("Critical Error : GetUserName\n");
		if (gethostname(localhost, 99) != 0) 
			throw("Critical Error : GetHostname\n");
		local = localhost;
	}
	catch (char* str) {
		std::cerr << str << std::endl;
		return ;
	}
	std::cout << "HELP : [server name] [GET|PUT] [filename path]" << std::endl
		<< "HELP : Type \"quit\" to exit" << std::endl << std::endl;

	while (1) {
		char	remotehost[100];
		char	direction[4];
		char	filename[200];
		char	cmd[304];

		std::cout << local.c_str() << ">";

		gets(cmd);
		if (!strcmp(cmd, "quit"))
			break ;
		sscanf(cmd, "%s %s %s", remotehost, direction, filename);
		if (strcmp(direction, "GET") && strcmp(direction, "PUT"))
			continue ;

		HOSTENT	*remote;
		if ((remote = gethostbyname(remotehost)) == NULL) {
			std::cerr << "[" << remotehost << "] unknown host" << std::endl;
			continue ;
		}

		try {
			if ((sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
				throw("Critical Error : socket init\n");
			
			memset(&ServerAddr, 0, sizeof(ServerAddr));
			ServerAddr.sin_family = AF_INET;
			memcpy(&ServerAddr.sin_addr,remote->h_addr,remote->h_length);
			ServerAddr.sin_family = remote->h_addrtype;
			ServerAddr.sin_port = htons(SERVER_PORT);
			if (connect(sock, (struct sockaddr *) &ServerAddr, sizeof(ServerAddr)) < 0)
				throw("Critical Error : socket connection\n");
		}
		catch (char *str) {
			std::cerr << str << std::endl;
			closesocket(sock);
			break ;
		}
		//fillMsg();
		
		try {
		//sendMsg(req);
		
		//receiveMsg();
		}
		catch (char *str) {
			std::cerr << str << std::endl;
			closesocket(sock);
			break ;
		}
		closesocket(sock);
	}
}