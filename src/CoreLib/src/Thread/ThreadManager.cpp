#include "ThreadManager.h"
#include "TLS.h"

#include <atomic>

using namespace std;
static atomic<unsigned int> SThreadId;

ThreadManager::ThreadManager()
{
	InitTLS();
}

ThreadManager::~ThreadManager()
{
	Join();
}

void ThreadManager::Launch(function<void(void)> callback)
{
	lock_guard<mutex> guard(lock);

	threads.push_back(thread([=]()
		{
			InitTLS();
			callback();
		}));
}

void ThreadManager::Join()
{
	for (thread& t : threads)
	{
		if (t.joinable())
			t.join();
	}
	threads.clear();
}

void ThreadManager::InitTLS()
{
	LThreadId = SThreadId.fetch_add(1);
}
