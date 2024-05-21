#pragma once

NAMESPACE_OPEN(NetCore);

class LoggerConfig;

extern class Memory* GMemory;
extern class SendBufferManager* GSendBufferManager;
extern class CoreLogger* GLogger;

#ifdef USE_GLOBAL_JOBQUEUE
extern class GlobalJobQueue* GGlobalJobQueue;
#endif // USE_GLOBAL_JOBQUEUE

void InitNetCore(const char* argv0, const std::string& logdir);
void InitNetCore(const char* argv0, const LoggerConfig* loggerConfig);

NAMESPACE_CLOSE