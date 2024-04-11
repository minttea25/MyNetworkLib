
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
	

	NetCore::Thread::TaskManager manager;
	auto t1 = manager.AddTask([]() {
		std::cout << "task 1" << std::endl;
		});
	auto t2 =  manager.AddTask([]() {
		this_thread::sleep_for(500ms);
		std::cout << "task 2" << std::endl;
		});
	auto t3 =  manager.AddTask([]() {
		std::cout << "task 3" << std::endl;
		});
	this_thread::sleep_for(200ms);
	std::cout << "Task1 end? " << manager.IsRunning(t1) << std::endl;
	std::cout << "Task2 end? " << manager.IsRunning(t2) << std::endl;
	std::cout << "Task3 end? " << manager.IsRunning(t3) << std::endl;

	this_thread::sleep_for(1000ms);
	cout << "end" << endl;
	return 0;
}
