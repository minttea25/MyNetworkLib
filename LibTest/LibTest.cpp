
#ifndef _DEBUG
pragma comment(lib, "MyNetworkLib\\Release\\MyNetworkLib.lib")

#else
#pragma comment(lib, "MyNetworkLib\\Debug\\MyNetworkLib.lib")

#endif // !_DEBUG

#include "CorePch.h"

#include <iostream>
#include <cmath>

using namespace std;

class C : public enable_shared_from_this<C>
{
public:
	C() : _id(-1) {}
	C(int v) : _id(v) {}
	void Print() const
	{
		cout << "C: " << _id << endl;
	}
	int Id() const { return _id; }
private:
	const int _id;
};

int main()
{
	auto num_cores = std::thread::hardware_concurrency();
	cout << num_cores << endl;
	{
		{
			NetCore::LockQueue<C> queue;

			NetCore::Thread::TaskManager manager;
			manager.AddTask([&queue]() {
				cout << "Producer: " << NetCore::TLS_Id << endl;
				for (int i = 0; i < 50; ++i)
				{
					queue.Push(C(i));
					this_thread::sleep_for(50ms);
				}
				});

			manager.AddTask([&queue]() {
				cout << "Consumer: " <<  NetCore::TLS_Id << endl;
				for (int i = 0; i < 50; ++i)
				{
					auto t = queue.Pop();
					if (t.Id() == -1) i--;
					else t.Print();
					this_thread::sleep_for(60ms);
				}
				});

			manager.JoinAllTasks();
		}
		cout << "----------------------------------------------------" << endl;
		{
			NetCore::LockStack<C> stack;

			NetCore::Thread::TaskManager manager;
			manager.AddTask([&stack]() {
				cout << "Producer: " << NetCore::TLS_Id << endl;
				for (int i = 0; i < 50; ++i)
				{
					stack.Push(C(i));
					this_thread::sleep_for(50ms);
				}
				});

			manager.AddTask([&stack]() {
				cout << "Consumer: " << NetCore::TLS_Id << endl;
				for (int i = 0; i < 50; ++i)
				{
					auto t = stack.Pop();
					if (t.Id() == -1) i--;
					else t.Print();
					this_thread::sleep_for(60ms);
				}
				});

			manager.JoinAllTasks();
		}
	}

	
	cout << "end" << endl;
	return 0;
}
