#include "pch.h"
#include "CoreGlobal.h"

#include "Memory.h"
#include "SendBufferManager.h"

NAMESPACE_OPEN(NetCore);

// extern instance used as global

Memory* GMemory = nullptr;
SendBufferManager* GSendBufferManager = nullptr;

#ifdef USE_GLOBAL_JOBQUEUE
GlobalJobQueue* GGlobalJobQueue = nullptr;
#endif // USE_GLOBAL_JOBQUEUE

class CoreGlobal
{
public:
	CoreGlobal()
	{
		// Note : Init GMemory first.
		// new constructor here
		GMemory = new Memory();
		GSendBufferManager = new SendBufferManager();
#ifdef USE_GLOBAL_JOBQUEUE
		GGlobalJobQueue = new GlobalJobQueue();
#endif // USE_GLOBAL_JOBQUEUE
		

		// init method here
		SocketUtils::Init();
	}
	~CoreGlobal()
	{
		// Note: Delete GMemory last.
		// delete here
#ifdef USE_GLOBAL_JOBQUEUE
		delete GGlobalJobQueue;
#endif // USE_GLOBAL_JOBQUEUE
		
		delete GSendBufferManager;
		delete GMemory;

		// clear method here
		SocketUtils::Clear();
	}
} GCoreGlobal;

NAMESPACE_CLOSE;