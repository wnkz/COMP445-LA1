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

	/*
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
	*/

	static const unsigned int SERVER_PORT = 21;

	static const unsigned int DIRECTION_LENGTH = 4;
	static const unsigned int HOSTNAME_LENGTH = 256;
	static const unsigned int FILENAME_LENGTH = 4096;

	static const unsigned int RESPONSE_LENGTH = 40;
	static const unsigned int BUFFER_LENGTH = 1024;

	static const unsigned int PACKET_HEADER_LENGTH = 8;
	static const unsigned int MAX_PACKET_LENGTH = PACKET_HEADER_LENGTH + BUFFER_LENGTH;

	enum Direction {
		GET = 1,
		PUT
	};

	enum Type {
		REQUEST = 1,
		RESPONSE,
		INITIAL_DATA,
		DATA,
		FINAL_DATA
	};

	enum ACK {
		YES = 1,
		NO
	};

	struct Request {
		Direction direction;
		char hostname[HOSTNAME_LENGTH];
		char filename[FILENAME_LENGTH];
	};

	struct Response{
		ACK acknowledge;
		char message[RESPONSE_LENGTH];
	};

	struct Packet {
		Type type;
		int buffer_length;
		char buffer[BUFFER_LENGTH];
	};

	void run(void);
	int ReceiveMessage(int s, TCPThread::Packet* mptr);
	int SendMessage(int s, TCPThread::Packet* mptr);


private:
	int cs;
};
