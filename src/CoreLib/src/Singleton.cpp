#include "Singleton.h"
#include "pch.h"

ThreadManager* GThreadManager = nullptr;
SendBufferManager* GSendBufferManager = nullptr;
LockQueue<shared_ptr<JobQueue>>* GPendingJobQueues = nullptr;
JobTimer* GJobTimer = nullptr;
shared_ptr<LogManager> GLogManager = nullptr;

class CoreGlobal
{
public:
	CoreGlobal()
	{
		GThreadManager = new ThreadManager();
		GSendBufferManager = new SendBufferManager();
		GPendingJobQueues = new LockQueue<shared_ptr<JobQueue>>();
		GJobTimer = new JobTimer();
		GLogManager = make_shared<LogManager>();
	}

	~CoreGlobal()
	{
		delete GThreadManager;
		delete GSendBufferManager;
		delete GPendingJobQueues;
		delete GJobTimer;
	}
} GCoreGlobal;