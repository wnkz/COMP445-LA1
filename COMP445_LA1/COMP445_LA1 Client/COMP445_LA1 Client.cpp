// COMP445_LA1 Client.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include "ClientFTP.h"


int _tmain(int argc, _TCHAR* argv[])
{
	ClientFTP*	client = new ClientFTP();
	client->run();
	return 0;
}

