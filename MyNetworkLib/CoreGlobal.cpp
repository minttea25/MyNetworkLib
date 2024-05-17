#include "pch.h"
#include "CoreGlobal.h"

#include "ErrorHandler.h"
#include "Memory.h"
#include "SendBufferManager.h"
#include "CoreLogger.h"

NAMESPACE_OPEN(NetCore);

// extern instance used as global

Memory* GMemory = nullptr;
SendBufferManager* GSendBufferManager = nullptr;
CoreLogger* GLogger = nullptr;

#ifdef USE_GLOBAL_JOBQUEUE
GlobalJobQueue* GGlobalJobQueue = nullptr;
#endif // USE_GLOBAL_JOBQUEUE

class CoreGlobal
{
public:
	CoreGlobal(const CoreGlobal&) = delete;
	CoreGlobal(const char* argv0)
	{
		GLogger = new CoreLogger(argv0);

		// Note : Init GMemory first.
		// new constructor here
		GMemory = new Memory();
		GSendBufferManager = new SendBufferManager();
#ifdef USE_GLOBAL_JOBQUEUE
		GGlobalJobQueue = new GlobalJobQueue();
#endif // USE_GLOBAL_JOBQUEUE
		

		// init method here
		ErrorHandler::InitErrorMap();
		SocketUtils::Init();
	}

	CoreGlobal(const char* argv0, const std::string& logdir)
	{
		GLogger = new CoreLogger(argv0, logdir);

		// Note : Init GMemory first.
		// new constructor here
		GMemory = new Memory();
		GSendBufferManager = new SendBufferManager();
#ifdef USE_GLOBAL_JOBQUEUE
		GGlobalJobQueue = new GlobalJobQueue();
#endif // USE_GLOBAL_JOBQUEUE


		// init method here
		ErrorHandler::InitErrorMap();
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
		delete GLogger;

		// clear method here
		SocketUtils::Clear();
	}
};

CoreGlobal* GCoreGlobal = nullptr;

/// <summary>
/// Init NetCore Library with argv0 of main().
/// <para>Note: argv0 is used for google::InitGoogleLoggin().</para>
/// <para>NetCore::ClearNetCore should be called when the app is closed.</para>
/// </summary>
/// <param name="argv0">program name for glog</param>
void InitNetCore(const char* argv0)
{
	GCoreGlobal = new CoreGlobal(argv0);
}

void InitNetCore(const char* argv0, const std::string& logdir)
{
	GCoreGlobal = new CoreGlobal(argv0, logdir);
}

/// <summary>
/// Clear NetCore memories.
/// </summary>
void ClearNetCore()
{
	delete GCoreGlobal;
}

NAMESPACE_CLOSE;