#include "Singleton.h"
#include "pch.h"

ThreadManager* GThreadManager = nullptr;
SendBufferManager* GSendBufferManager = nullptr;
GlobalQueue* GGlobalQueue = nullptr;
JobTimer* GJobTimer = nullptr;
shared_ptr<LogManager> GLogManager = nullptr;

class CoreGlobal
{
public:
	CoreGlobal()
	{
		GThreadManager = new ThreadManager();
		GSendBufferManager = new SendBufferManager();
		GGlobalQueue = new GlobalQueue();
		GJobTimer = new JobTimer();
		GLogManager = make_shared<LogManager>();
	}

	~CoreGlobal()
	{
		delete GThreadManager;
		delete GSendBufferManager;
		delete GGlobalQueue;
		delete GJobTimer;
	}
} GCoreGlobal;