#include "pch.h"
#include "CoreGlobal.h"

#include "Memory.h"
#include "SendBufferManager.h"

NAMESPACE_OPEN(NetCore);

// extern instance used as global
Memory* GMemory = nullptr;
SendBufferManager* GSendBufferManager = nullptr;
GlobalJobQueue* GGlobalJobQueue = nullptr;

class CoreGlobal
{
public:
	CoreGlobal()
	{
		// Note : Init GMemory first.
		// new constructor here
		GMemory = new Memory();
		GSendBufferManager = new SendBufferManager();
		GGlobalJobQueue = new GlobalJobQueue();

		// init method here
		SocketUtils::Init();
	}
	~CoreGlobal()
	{
		// Note: Delete GMemory last.
		// delete here
		delete GGlobalJobQueue;
		delete GSendBufferManager;
		delete GMemory;

		// clear method here
		SocketUtils::Clear();
	}
} GCoreGlobal;

NAMESPACE_CLOSE;