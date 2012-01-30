
#include "StdAfx.h"
#include "client_tcp.h"

int main(int argc, char *argv[]) {

	TcpClient *client = new TcpClient();
	client->run();
	return 0;
}