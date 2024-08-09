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
	/*{
		auto& rJobRef1 = room->PushReservableJob(1000, &Room::Print, 1);
		auto& rJobRef2 = room->PushReservableJob(500, &Room::Print, 2);
		auto& rJobRef3 = room->PushReservableJob(1500, &Room::Print, 3);
	}*/

	manager.AddTask([&] {
		bool bin = 0;
		while (true)
		{
			auto& rJobRef = room->PushReservableJob(100, &Room::Reserve);
			
			if (bin == 0)
			{
				rJobRef.Cancel();
				bin = 1;
			}
			else bin = 0;

			this_thread::sleep_for(50ms);

			room->AddJob();
			room->AddJob();
		}
		});

	manager.JoinAllTasks();

	return 0;	
}