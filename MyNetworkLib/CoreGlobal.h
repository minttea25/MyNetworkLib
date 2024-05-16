#pragma once

NAMESPACE_OPEN(NetCore);
extern class Memory* GMemory;
extern class SendBufferManager* GSendBufferManager;
extern class CoreLogger* GLogger;

#ifdef USE_GLOBAL_JOBQUEUE
extern class GlobalJobQueue* GGlobalJobQueue;
#endif // USE_GLOBAL_JOBQUEUE

extern class CoreGlobal* GCoreGlobal;

void InitNetCore(const char* argv0);
void InitNetCore(const char* argv0, const std::string& logdir);
void ClearNetCore();

NAMESPACE_CLOSE