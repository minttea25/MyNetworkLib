#pragma once

class Sample
{
public:
	void PrintHello()
	{
		std::cout << "Hello, world!" << '\n';
	}

	void PrintGlog(char* argv[])
	{
		// See: https://rpg.ifi.uzh.ch/docs/glog.html

		// FLAGS_log_dir 값 변경은 InitGoogleLogging 전에 해야됨
		FLAGS_log_dir = "../TestLogs";

		google::InitGoogleLogging(argv[0]);

		// FLAGS_log_dir에 설정된 값과 상관 없이 생성
		//google::SetLogDestination(google::GLOG_INFO, "./TestLogs/test_log_info");

		FLAGS_logtostderr = false; // true면 파일 생성하지 않음
		LOG(INFO) << "[FLAGS_logtostderr = false] This is test log! 1 ";
		LOG(INFO) << "[FLAGS_logtostderr = false] This is test log! 2";
		LOG(INFO) << "[FLAGS_logtostderr = false] This is test log! 3";

		FLAGS_logtostderr = true;
		LOG(INFO) << "[FLAGS_logtostderr = true] This is test log! 1 ";
		LOG(INFO) << "[FLAGS_logtostderr = true] This is test log! 2";
		LOG(INFO) << "[FLAGS_logtostderr = true] This is test log! 3";

		google::ShutdownGoogleLogging();
	}
};

