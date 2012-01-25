#include "StdAfx.h"
#include "ftpd_tcp.h"


ftpd_tcp::ftpd_tcp(void)
{

}


ftpd_tcp::~ftpd_tcp(void)
{
}

void ftpd_tcp::startMessage(void)
{
	std::cout << "ftpd_tcp starting at host: " << this->getHostname().c_str() << std::endl;
	std::cout << "waiting to be contacted for transfering files..." << std::endl;
}

std::string ftpd_tcp::getHostname(void)
{
	WSAData wdata;
	std::string hostname;
	char hname[256];
	int r;

	WSAStartup(MAKEWORD(2, 2), &wdata);

	r = gethostname(hname, sizeof(hname));
	if (r == SOCKET_ERROR)
	{
		//TODO: Error checking.
		hostname = "Unknown";
	}
	else
	{
		hostname = hname;
	}

	WSACleanup();

	return hostname;
 }