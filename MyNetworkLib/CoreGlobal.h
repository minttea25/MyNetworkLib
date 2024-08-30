#pragma once

NAMESPACE_OPEN(NetCore);

struct LoggerConfig;

/// <summary>
/// Memory class used in library
/// </summary>
extern class Memory* GMemory;

/// <summary>
/// Provider of sendbuffer in IOCPCore
/// </summary>
extern class SendBufferManager* GSendBufferManager;

/// <summary>
/// Logger object used in library (glog)
/// </summary>
extern class CoreLogger* GLogger;

#ifdef USE_GLOBAL_JOB_SERIALIZER
/// <summary>
/// JobWorker used as global in library
/// </summary>
extern class GlobalJobWorker* GGlobalJobWorker;
#endif // USE_GLOBAL_JOBQUEUE

/// <summary>
/// Init NetCore Library with argv0 of main().
/// <para>Note: argv0 is used for google::InitGoogleLoggin().</para>
/// <para>NetCore::ClearNetCore should be called when the app is closed.</para>
/// </summary>
/// <param name="argv0">program name for glog</param>
/// <param name="logdir">location to create log files</param>
void InitNetCore(const char* argv0, const std::string& logdir);
void InitNetCore(const char* argv0, const LoggerConfig* loggerConfig);

NAMESPACE_CLOSE