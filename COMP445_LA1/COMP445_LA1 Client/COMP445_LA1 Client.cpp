// COMP445_LA1 Client.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"


int _tmain(int argc, _TCHAR* argv[])
{
	TcpClient *client = new TcpClient();

	client->run();
	return 0;
}

