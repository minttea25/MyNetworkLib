#include "pch.h"
#include "CoreGlobal.h"

#include "Memory.h"

NAMESPACE_OPEN(NetCore);

// extern instance used as global
Memory* GMemory = nullptr;

class CoreGlobal
{
public:
	CoreGlobal()
	{
		// new constructor here
		GMemory = new NetCore::Memory();

		// init method here
		SocketUtils::Init();
	}
	~CoreGlobal()
	{
		// delete here
		delete GMemory;

		// clear method here
		SocketUtils::Clear();
	}
} GCoreGlobal;

NAMESPACE_CLOSE;