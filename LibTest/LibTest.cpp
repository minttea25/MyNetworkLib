#ifndef _DEBUG

#pragma comment(lib, "MyNetworkLib\\Release\\MyNetworkLib.lib")



#else
#pragma comment(lib, "MyNetworkLib\\Debug\\MyNetworkLib.lib")


#endif // !_DEBUG

#include "CorePch.h"

using namespace std;

class Room : public NetCore::JobSerializer
{
public:
	~Room() { std::cout << "~Room" << std::endl; }
	void AddJob()
	{
		PushJob([&] {
			n++;
			std::cout << NetCore::TLS_Id << " - Job: " << ::GetTickCount64() % 10000 << " n: " << n << std::endl;
			});
	}

	void Reserve()
	{
		n--;
		std::cout << NetCore::TLS_Id << " - RJob: " << ::GetTickCount64() % 10000 << " n: " << n << std::endl;
	}

	void Print(int value)
	{
		std::cout << NetCore::TLS_Id << " Print: " << value << std::endl;
	}

	int n = 0;
};

int main(int argc, char* argv)
{
	NetCore::InitNetCore(argv, "../TestLogs");

	{
		google::LogToStderr();
	}

	std::shared_ptr<Room> room = NetCore::make_shared<Room>();

	NetCore::TaskManagerEx manager;
	manager.AddTask([&] {
		while (true)
		{
			NetCore::TLS_GlobalJobsExecutionMaxTickCount = 100; // (1s)

			manager.DoWorkJob();

			manager.CheckReservedJob();
		}
		}, 3);

	// for check priority queue
	{
		auto job1 = NetCore::make_shared< NetCore::Job>(room, &Room::Print, 1);
		auto& rJobRef1 = NetCore::GGlobalJobWorker->AddReservableJob(1000, job1, room);

		auto job2 = NetCore::make_shared< NetCore::Job>(room, &Room::Print, 2);
		auto& rJobRef2 = NetCore::GGlobalJobWorker->AddReservableJob(500, job2, room);

		auto job3 = NetCore::make_shared< NetCore::Job>(room, &Room::Print, 3);
		auto& rJobRef3 = NetCore::GGlobalJobWorker->AddReservableJob(1500, job3, room);
	}

	manager.AddTask([&] {
		bool bin = 0;
		while (true)
		{
			auto job = NetCore::make_shared< NetCore::Job>(room, &Room::Reserve);
			auto& rJobRef = NetCore::GGlobalJobWorker->AddReservableJob(100, job, room);
			
			if (bin == 0)
			{
				rJobRef.Cancel();
				bin = 1;
			}
			else bin = 0;

			this_thread::sleep_for(300ms);

			room->AddJob();
			room->AddJob();
		}
		});

	manager.JoinAllTasks();

	return 0;	
}