
#ifndef _DEBUG
pragma comment(lib, "MyNetworkLib\\Release\\MyNetworkLib.lib")

#else
#pragma comment(lib, "MyNetworkLib\\Debug\\MyNetworkLib.lib")

#endif // !_DEBUG

#include "CorePch.h"

#include <iostream>
#include <cmath>

using namespace std;

int main()
{
	auto num_cores = std::thread::hardware_concurrency();

	cout << num_cores << endl;


	{
		NetCore::Thread::TaskManager manager;
		auto id = manager.AddTask([]() { cout << "task 1" << endl; });
		auto ids = manager.AddTask([]() { while (true) { cout << "infinite task" << endl; this_thread::sleep_for(500ms); }}, 2);
	
		
		cout << "Join? : " << manager.JoinTask(ids.first) << endl;
		cout << "Join? : " << manager.JoinTask(ids.second) << endl;

		this_thread::sleep_for(1000ms);
		cout << "Join? : " << manager.JoinTask(id) << endl;
	}

	
	cout << "end" << endl;
	return 0;
}
