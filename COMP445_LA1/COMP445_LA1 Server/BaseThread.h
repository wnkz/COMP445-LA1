#pragma once
#include <process.h>
#include <iostream>
#include <cstdlib>

class BaseThread
{
public:
	BaseThread(void);
	virtual ~BaseThread(void);

	static void* pthread_callback(void* p);
	virtual void run(void) = 0;

	void start();

private:
	static const int STKSIZE = 16536;
};

