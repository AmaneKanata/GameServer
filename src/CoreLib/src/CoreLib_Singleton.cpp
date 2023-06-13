#include "CoreLib_Singleton.h"
#include "ThreadManager.h"
#include "SendBuffer.h"

ThreadManager* GThreadManager = nullptr;
SendBufferManager* GSendBufferManager = nullptr;

class CoreGlobal
{
public:
	CoreGlobal()
	{
		GThreadManager = new ThreadManager();
		GSendBufferManager = new SendBufferManager();
	}

	~CoreGlobal()
	{
		delete GThreadManager;
		delete GSendBufferManager;
	}
} GCoreGlobal;