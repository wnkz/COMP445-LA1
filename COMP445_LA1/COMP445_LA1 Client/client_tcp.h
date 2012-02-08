#pragma once

#include <winsock2.h>

static const unsigned int SERVER_PORT = 21;

static const unsigned int DIRECTION_LENGTH = 4;
static const unsigned int HOSTNAME_LENGTH = 256;
static const unsigned int FILENAME_LENGTH = 4096;

static const unsigned int RESPONSE_LENGTH = 40;
static const unsigned int BUFFER_LENGTH = 1024;

static const unsigned int PACKET_HEADER_LENGTH = 8;
static const unsigned int MAX_PACKET_LENGTH = PACKET_HEADER_LENGTH + BUFFER_LENGTH;

enum Direction{
	GET=1,
	PUT
};

enum Type {
	REQUEST=1,
	RESPONSE,
	INITIAL_DATA,
	DATA,
	FINAL_DATA
};

enum ACK {
	YES=1,
	NO
};

typedef struct {
	Direction direction;
	char hostname[HOSTNAME_LENGTH];
	char filename[FILENAME_LENGTH];
} Request;

typedef struct {
	ACK acknowledge;
	char message[RESPONSE_LENGTH];
} Response;

typedef struct {
	Type type;
	int buffer_length;
	char buffer[BUFFER_LENGTH];
} Packet;

class TcpClient {
public:
	TcpClient();
	~TcpClient();

	void			run();

	void			sendFile(char *filename);
	void			receiveFile(char *filename);

	void			sendPacket(Packet *p);
	Packet			*receivePacket();

protected:
private:
	int					sock;
	struct sockaddr_in	ServerAddr;
	WSADATA				wsadata;
};