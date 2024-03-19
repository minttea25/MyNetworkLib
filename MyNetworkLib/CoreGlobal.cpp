#include "pch.h"
#include "CoreGlobal.h"

#include "Memory.h"


NAMESPACE_OPEN(NetCore)

CoreGlobal GCoreGlobal;

// extern instance used as global
Memory* GMemory = nullptr;

NAMESPACE_CLOSE

NetCore::CoreGlobal::CoreGlobal()
{
	// new constructor here
	GMemory = new NetCore::Memory();

	// init method here
}

NetCore::CoreGlobal::~CoreGlobal()
{
	// delete here
	delete GMemory;

	// clear method here

}
