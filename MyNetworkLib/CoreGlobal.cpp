#include "pch.h"
#include "CoreGlobal.h"

#include "ErrorHandler.h"
#include "Memory.h"
#include "SendBufferManager.h"
#include "GlobalJobWorker.h"
#include "CoreLogger.h"

NAMESPACE_OPEN(NetCore);

// extern instance used as global

/// <summary>
/// Memory class used in library
/// </summary>
Memory* GMemory = nullptr;

/// <summary>
/// Provider of sendbuffer in IOCPCore
/// </summary>
SendBufferManager* GSendBufferManager = nullptr;

/// <summary>
/// Logger object used in library (glog)
/// </summary>
CoreLogger* GLogger = nullptr;

#ifdef USE_GLOBAL_JOB_SERIALIZER
/// <summary>
/// JobWorker used as global in library
/// </summary>
GlobalJobWorker* GGlobalJobWorker = nullptr;
#endif // USE_GLOBAL_JOBQUEUE

class CoreGlobal
{
public:
	CoreGlobal(const CoreGlobal&) = delete;

	CoreGlobal()
	{
	}

	~CoreGlobal()
	{
		// Note: Delete GMemory last.
		// delete here
#ifdef USE_GLOBAL_JOB_SERIALIZER
		delete GGlobalJobWorker;
#endif // USE_GLOBAL_JOBQUEUE
		
		delete GSendBufferManager;
		delete GMemory;
		delete GLogger;

		// clear method here
		SocketUtils::Clear();
	}
public:
	void _init(const char* argv0, const std::string& logdir)
	{
		GLogger = new CoreLogger(argv0, logdir);

		// Note : Init GMemory first.
		// new constructor here
		GMemory = new Memory();
		GSendBufferManager = new SendBufferManager();
#ifdef USE_GLOBAL_JOB_SERIALIZER
		GGlobalJobWorker = new GlobalJobWorker();
#endif // USE_GLOBAL_JOBQUEUE


		// init method here
		ErrorHandler::InitErrorMap();
		SocketUtils::Init();
	}

	void _init(const char* argv0, const LoggerConfig* loggerConfig)
	{
		GLogger = new CoreLogger(argv0, loggerConfig);

		// Note : Init GMemory first.
		// new constructor here
		GMemory = new Memory();
		GSendBufferManager = new SendBufferManager();
#ifdef USE_GLOBAL_JOB_SERIALIZER
		GGlobalJobWorker = new GlobalJobWorker();
#endif // USE_GLOBAL_JOBQUEUE


		// init method here
		ErrorHandler::InitErrorMap();
		SocketUtils::Init();
	}
} GCoreGlobal;

/// <summary>
/// Init NetCore Library with argv0 of main().
/// <para>Note: argv0 is used for google::InitGoogleLoggin().</para>
/// <para>NetCore::ClearNetCore should be called when the app is closed.</para>
/// </summary>
/// <param name="argv0">program name for glog</param>
/// <param name="logdir"></param>
void InitNetCore(const char* argv0, const std::string& logdir)
{
	GCoreGlobal._init(argv0, logdir);
}

void InitNetCore(const char* argv0, const LoggerConfig* loggerConfig)
{
	GCoreGlobal._init(argv0, loggerConfig);
}

NAMESPACE_CLOSE;