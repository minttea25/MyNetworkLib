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

#ifdef USE_GLOBAL_JOB_SERIALIZER
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