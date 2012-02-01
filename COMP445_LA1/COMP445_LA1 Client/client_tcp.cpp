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
		   /!\Fill Packet(first ? INITIAL_DATA : DATA)
	   }
	   sendPacket(req);
	   first = false;
     }
	 sendPacket for finish
     fclose (stream);
	 */
}

void	TcpClient::receiveFile() {

}

void	TcpClient::fillPacket() {

}

void	TcpClient::sendPacket(Packet *p) {
	int size;
	size = send(sock,(char *)p, p->buffer_length + PACKET_HEADER_LENGTH, 0);
	if (size == -1)
		throw("Error : send\n");
}

Packet*	TcpClient::receivePacket() {
	int	size;
	char buf[MAX_PACKET_LENGTH];
	Packet *p = new Packet;

	size = recv(sock, buf, sizeof(buf), 0);
	if (size == -1 || size < PACKET_HEADER_LENGTH)
		throw("Error : recv\n");
	memcpy((char *)p->type, buf, PACKET_HEADER_LENGTH);
	p->buffer_length = size - PACKET_HEADER_LENGTH;
	memcpy((char *)p->buffer, &buf[PACKET_HEADER_LENGTH], p->buffer_length);
	return p;
}

void	TcpClient::run() {
	std::string	local;

	try {
		if (WSAStartup(0x0202, &wsadata) != 0) {  
			WSACleanup();  
			throw("Critical Error : WSAStartup\n");
		}
		char	localhost[HOSTNAME_LENGTH];
		if (gethostname(localhost, HOSTNAME_LENGTH - 1) != 0) 
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
		char	remotehost[HOSTNAME_LENGTH];
		char	direction[4];
		char	filename[FILENAME_LENGTH];
		char	cmd[HOSTNAME_LENGTH + DIRECTION_LENGTH + FILENAME_LENGTH + 3];

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
			memcpy(&ServerAddr.sin_addr, remote->h_addr, remote->h_length);
			ServerAddr.sin_family = remote->h_addrtype;
			ServerAddr.sin_port = htons(SERVER_PORT);
			if (connect(sock, (struct sockaddr *) &ServerAddr, sizeof(ServerAddr)) < 0)
				throw("Error : socket connection\n");
		}
		catch (char *str) {
			std::cerr << str << std::endl;
			closesocket(sock);
			continue ;
		}
		Request request;
		memcpy(request.hostname, local.c_str(), local.length());
		memcpy(request.filename, filename, sizeof(filename));
		request.direction = (!strcmp(direction, "GET") ? GET : PUT);

		Packet p;
		p.type = REQUEST;
		memcpy(p.buffer, &request, sizeof(request));
		p.buffer_length = sizeof(request);
		
		try {
		sendPacket(&p);
		receivePacket();
		}
		catch (char *str) {
			std::cerr << str << std::endl;
			closesocket(sock);
			continue ;
		}
		closesocket(sock);
	}
}