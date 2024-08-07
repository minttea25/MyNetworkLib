#pragma once

NAMESPACE_OPEN(NetCore);

struct LoggerConfig
{
	bool LogToStderr = false;
	std::string DirPath;
	std::string InfoPathPrefix;
	std::string ErrorPathPrefix;
	std::string FatalPathPrefix;
	std::string WarningPathPrefix;
};

class CoreLogger
{
	constexpr static bool LOG_TO_STDERR = false;
	constexpr static char INFO_PATH[] = "INFO_";
	constexpr static char ERROR_PATH[] = "ERROR_";
	constexpr static char FATAL_PATH[] = "FATAL_";
	constexpr static char WARNING_PATH[] = "WARNING_";

public:
	CoreLogger(const CoreLogger&) = delete;

	CoreLogger(const char* name)
	{
		google::InitGoogleLogging(name);

		FLAGS_logtostderr = LOG_TO_STDERR;

		google::SetLogDestination(google::GLOG_INFO, INFO_PATH);
		google::SetLogDestination(google::GLOG_ERROR, ERROR_PATH);
		google::SetLogDestination(google::GLOG_WARNING, WARNING_PATH);
		google::SetLogDestination(google::GLOG_FATAL, FATAL_PATH);
	}

	CoreLogger(const char* name, const std::string& logdir)
	{
		google::InitGoogleLogging(name);

		FLAGS_logtostderr = LOG_TO_STDERR;

		_set_log_path(google::GLOG_INFO, logdir, INFO_PATH);
		_set_log_path(google::GLOG_ERROR, logdir, ERROR_PATH);
		_set_log_path(google::GLOG_WARNING, logdir, WARNING_PATH);
		_set_log_path(google::GLOG_FATAL, logdir, FATAL_PATH);
	}

	CoreLogger(const char* name, const LoggerConfig* config)
	{
		google::InitGoogleLogging(name);

		FLAGS_logtostderr = config->LogToStderr;

		_set_log_path(google::GLOG_INFO, config->DirPath, config->InfoPathPrefix);
		_set_log_path(google::GLOG_ERROR, config->DirPath, config->ErrorPathPrefix);
		_set_log_path(google::GLOG_WARNING, config->DirPath, config->WarningPathPrefix);
		_set_log_path(google::GLOG_FATAL, config->DirPath, config->FatalPathPrefix);
	}

	~CoreLogger()
	{
		google::ShutdownGoogleLogging();
	}

private:
	void _set_log_path(google::LogSeverity severity, const std::string& dirPath, const std::string& pathPrefix)
	{
		std::string fullPath = dirPath + "/" + pathPrefix;
		google::SetLogDestination(severity, fullPath.c_str());
	}
};

NAMESPACE_CLOSE;

