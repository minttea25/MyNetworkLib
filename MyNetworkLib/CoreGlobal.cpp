#include "pch.h"
#include "CoreGlobal.h"

#include "Memory.h"
#include "SendBufferManager.h"

NAMESPACE_OPEN(NetCore);

// extern instance used as global
Memory* GMemory = nullptr;
SendBufferManager* GSendBufferManager = nullptr;

class CoreGlobal
{
public:
	CoreGlobal()
	{
		// new constructor here
		GMemory = new Memory();
		GSendBufferManager = new SendBufferManager();

		// init method here
		SocketUtils::Init();
	}
	~CoreGlobal()
	{
		// delete here
		delete GMemory;
		delete GSendBufferManager;

		// clear method here
		SocketUtils::Clear();
	}
} GCoreGlobal;

NAMESPACE_CLOSE;