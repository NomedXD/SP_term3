#ifndef THREADPOOL_H
#define THREADPOOL_H

#include "Queue.h"

#define SPINS 4000

class ThreadPool
{
public:

	ThreadPool(int thread_quantity, Queue* task_queue);
	void WaitAll();
	~ThreadPool();
private:
	HANDLE* threads;
	CRITICAL_SECTION csection;
	int threadsCount;
	static DWORD WINAPI DoThreadWork(LPVOID obj);
};

#endif
