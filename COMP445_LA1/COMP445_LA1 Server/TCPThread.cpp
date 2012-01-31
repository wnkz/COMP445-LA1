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
	TCPThread::Message smsg, rmsg;
	struct _stat sbuff;

	if (ReceiveMessage(cs, &rmsg) != rmsg.length) {
		std::cerr << "ReceiveMessage(): error" << std::endl;
	}

	rq = (TCPThread::Request*)rmsg.buffer;
	std::cout << "Received a request from client: " << rq->hostname;

	smsg.type = TCPThread::Type::RESP;
	smsg.length = sizeof(rp);

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

	if (SendMessage(cs, &smsg) != smsg.length) {
		std::cerr << "SendMessage(): error" << std::endl;
	}

	std::cout << "Response for " << rq->hostname << " has been sent out" << std::endl;

	closesocket(cs);
}

int TCPThread::ReceiveMessage(int s, TCPThread::Message* mptr)
{
	int rbytes;
	int n;

	n = 0;
	for (rbytes = 0; rbytes < TCPThread::MESSAGE_HEADER_SIZE; rbytes += n) {
		if ((n = recv(s, (char*)mptr + rbytes, TCPThread::MESSAGE_HEADER_SIZE - rbytes, 0)) <= 0) {
			std::cerr << "recv(): error" << std::endl;
			break;
		}
	}

	n = 0;
	for (rbytes = 0; rbytes < mptr->length; rbytes += n) {
		if ((n = recv(s, (char*)mptr->buffer + rbytes, mptr->length - rbytes, 0)) <= 0) {
			std::cerr << "recv(): error" << std::endl;
			break;
		}
	}
	
	return mptr->length;
}

int TCPThread::SendMessage(int s, TCPThread::Message* mptr)
{
	int n;

	if ((n = send(s, (char*)mptr, TCPThread::MESSAGE_HEADER_SIZE + mptr->length, 0)) != (TCPThread::MESSAGE_HEADER_SIZE + mptr->length)) {
		std::cerr << "send(): error" << std::endl;
	}

	return (n - TCPThread::MESSAGE_HEADER_SIZE);
}
