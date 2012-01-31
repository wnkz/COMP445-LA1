#pragma once
#include <iostream>
#include <process.h>


class myThread
{
public:
	myThread(void);
	virtual ~myThread(void);

	static void* pthread_callback(void* p);
	virtual void run(void) = 0;
	void start();

private:
	static const int STKSIZE = 16536;
};

