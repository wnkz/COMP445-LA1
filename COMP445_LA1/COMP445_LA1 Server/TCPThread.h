#pragma once
#include <time.h>
#include <sys/stat.h>

#include "mythread.h"


class TCPThread :
	public myThread
{
public:
	TCPThread(int clientSocket);
	~TCPThread(void);

	static const unsigned int HOSTNAME_LENGTH = 256;
	static const unsigned int FILENAME_LENGTH = 4096;
	static const unsigned int RESPONSE_LENGTH = 40;
	static const unsigned int BUFFER_LENGTH = 1024;
	static const unsigned int MESSAGE_HEADER_SIZE = 8;

	enum Type
	{
		REQ_SIZE = 1,
		REQ_TIME,
		RESP
	};

	struct Request
	{
		char hostname[HOSTNAME_LENGTH];
		char filename[FILENAME_LENGTH];
	};

	struct Response
	{
		char response[RESPONSE_LENGTH];
	};

	struct Message
	{
		TCPThread::Type type;
		int length;
		char buffer[BUFFER_LENGTH];
	};

	void run(void);
	int ReceiveMessage(int s, TCPThread::Message* mptr);
	int SendMessage(int s, TCPThread::Message* mptr);


private:
	int cs;
};
