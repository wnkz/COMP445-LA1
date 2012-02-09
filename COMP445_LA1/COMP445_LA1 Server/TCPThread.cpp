#include "StdAfx.h"
#include "TCPThread.h"


TCPThread::TCPThread(int clientSocket) : cs(clientSocket)
{
}


TCPThread::~TCPThread(void)
{
}

void TCPThread::run(void)
{
	TCPThread::Response rp;
	TCPThread::Request* rq;
	TCPThread::Packet smsg, rmsg;
	struct _stat sbuff;

	if (ReceiveMessage(cs, &rmsg) != rmsg.buffer_length) {
		std::cerr << "ReceiveMessage(): error" << std::endl;
	}

	rq = (TCPThread::Request*)rmsg.buffer;
	std::cout << "Received a request from client: " << rq->hostname;

	/*
	smsg.type = TCPThread::Type::RESP;
	smsg.buffer_length = sizeof(rp);

	if (_stat(rq->filename, &sbuff) != 0) {
		sprintf(rp.response, "No such file");
	} else {
		memset(rp.response, 0, sizeof(rp));
		if (rmsg.type == TCPThread::Type::REQ_TIME) {
			sprintf(rp.response, "%s", ctime(&sbuff.st_ctime));
		} else if (rmsg.type == TCPThread::Type::REQ_SIZE) {
			sprintf(rp.response, "File size: %ld", sbuff.st_size);
		}
	}

	memcpy(smsg.buffer, &rp, sizeof(rp));

	if (SendMessage(cs, &smsg) != smsg.buffer_length) {
		std::cerr << "SendMessage(): error" << std::endl;
	}

	std::cout << "Response for " << rq->hostname << " has been sent out" << std::endl;

	closesocket(cs);
	*/
}

int TCPThread::ReceiveMessage(int s, TCPThread::Packet* p)
{
	int r;
	char buf[TCPThread::MAX_PACKET_LENGTH];
	//Packet* p = new Packet();

	r = recv(s, buf, sizeof(buf), 0);
	if (r <= 0) {
		std::cerr << "recv(): error" << std::endl;
		return -1;
	}

	memcpy((char*)p->buffer, &buf[TCPThread::PACKET_HEADER_LENGTH], p->buffer_length);
	p->buffer_length = r - TCPThread::PACKET_HEADER_LENGTH;
	memcpy((char*)p->buffer, &buf[TCPThread::PACKET_HEADER_LENGTH], p->buffer_length);
	return r;

	/*
	int rbytes;
	int n;

	n = 0;
	for (rbytes = 0; rbytes < TCPThread::PACKET_HEADER_LENGTH; rbytes += n) {
		if ((n = recv(s, (char*)mptr + rbytes, TCPThread::PACKET_HEADER_LENGTH - rbytes, 0)) <= 0) {
			std::cerr << "recv(): error" << std::endl;
			break;
		}
	}

	n = 0;
	for (rbytes = 0; rbytes < mptr->buffer_length; rbytes += n) {
		if ((n = recv(s, (char*)mptr->buffer + rbytes, mptr->buffer_length - rbytes, 0)) <= 0) {
			std::cerr << "recv(): error" << std::endl;
			break;
		}
	}
	
	return mptr->buffer_length;
	*/
}

int TCPThread::SendMessage(int s, TCPThread::Packet* mptr)
{
	int n;

	if ((n = send(s, (char*)mptr, TCPThread::PACKET_HEADER_LENGTH + mptr->buffer_length, 0)) != (TCPThread::PACKET_HEADER_LENGTH + mptr->buffer_length)) {
		std::cerr << "send(): error" << std::endl;
	}

	return (n - TCPThread::PACKET_HEADER_LENGTH);
}
