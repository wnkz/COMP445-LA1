#include "StdAfx.h"
#include "myThread.h"


myThread::myThread(void)
{
}


myThread::~myThread(void)
{
}

void* myThread::pthread_callback(void* p)
{
	if (p == NULL) {
		return NULL;
	}
	
	((myThread*)p)->run();
	return NULL;
}

void myThread::start(void)
{
	int r;

	if ((r = _beginthread((void (*)(void *))myThread::pthread_callback, STKSIZE, this) < 0)) {
		std::cerr << "_beginthread(): error" << std::endl;
	}
}