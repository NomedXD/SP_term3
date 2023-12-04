#include "ThreadPool.h"

ThreadPool::ThreadPool(int thread_quantity, Queue* task_queue)
{

	threads = (HANDLE*)malloc(sizeof(HANDLE) * thread_quantity);
	this->threadsCount = thread_quantity;
	InitializeCriticalSection(&csection);
	for (int i = 0; i < thread_quantity; i++)
		threads[i] = CreateThread(NULL, 0, &ThreadPool::DoThreadWork, (LPVOID)task_queue, NULL, NULL);
}

void ThreadPool::WaitAll()
{
	WaitForMultipleObjects(threadsCount, threads, TRUE, INFINITE);
}

ThreadPool::~ThreadPool()
{
	DeleteCriticalSection(&csection);
}

DWORD WINAPI ThreadPool::DoThreadWork(LPVOID obj)
{
	Queue* tasks = (Queue*)obj;
	while (1)
	{
		Task* tmp = tasks->retrieve();
		if (tmp == NULL)
			ExitThread(0);
		tmp->func(tmp->params);
	}
	return 0;

}

