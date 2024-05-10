#pragma once

NAMESPACE_OPEN(NetCore);
extern class Memory* GMemory;
extern class SendBufferManager* GSendBufferManager;

#ifdef USE_GLOBAL_JOBQUEUE
extern class GlobalJobQueue* GGlobalJobQueue;
#endif // USE_GLOBAL_JOBQUEUE



NAMESPACE_CLOSE