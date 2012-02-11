#include "StdAfx.h"
#include "BaseThread.h"


BaseThread::BaseThread(void)
{
}


BaseThread::~BaseThread(void)
{
}

void* BaseThread::pthread_callback(void* p)
{
	if (p == NULL) {
		return NULL;
	}
	
	((BaseThread*)p)->run();
	return NULL;
}

void BaseThread::start(void)
{
	int r;

	if ((r = _beginthread((void (*)(void *))BaseThread::pthread_callback, STKSIZE, this) < 0)) {
		std::cerr << "_beginthread(): error" << std::endl;
		exit(EXIT_FAILURE);
	}
}
