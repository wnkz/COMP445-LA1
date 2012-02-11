// COMP445_LA1 Server.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "FTPd.h"

int _tmain(int argc, _TCHAR* argv[])
{
	FTPd* f = new FTPd();

	f->start();
	return 0;
}

