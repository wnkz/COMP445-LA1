// COMP445_LA1 Server.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"


int _tmain(int argc, _TCHAR* argv[])
{
	ftpd_tcp *f = new ftpd_tcp();

	f->run();
	return 0;
}

