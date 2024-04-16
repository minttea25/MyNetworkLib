
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

class A : public NetCore::JobSerializer
{
public:
	A(int v) : _a(v) {}
	~A() { std::cout << "~A" << std::endl; }
	void Multifly(const int x)
	{
		_a *= x;
		std::cout << "now a:" << _a << std::endl;
	}
private:
	int _a;
};

class B : public NetCore::JobSerializerWithTimer
{
public:
	B(int v) : _b(v) {}
	~B() { std::cout << "~B" << std::endl; }
	void Multifly(const int x)
	{
		_b *= x;
		std::cout << "now b:" << _b << std::endl;
	}
private:
	int _b;
};

int main()
{
	auto num_cores = std::thread::hardware_concurrency();

	cout << num_cores << endl;

	{
		NetCore::Thread::TaskManager manager;
		shared_ptr<A> serializer = NetCore::make_shared<A>(10);

		manager.AddTask([&serializer]() {
			for (int i = 0; i < 5; ++i)
			{
				serializer->PushJob(&A::Multifly, 2);
				this_thread::sleep_for(120ms);
			}
			});
		manager.AddTask([&serializer]() {
			for (int i = 0; i < 5; ++i)
			{
				serializer->PushJob(&A::Multifly, 3);
				this_thread::sleep_for(200ms);
			}
			});
		manager.AddTask([&serializer]() {
			auto begin = ::GetTickCount64();
			while (true)
			{
				serializer->Flush();
				if (::GetTickCount64() - begin >= 10000) break;
			}
			});
		manager.JoinAllTasks();
	}
	{
		NetCore::Thread::TaskManager manager;
		shared_ptr<B> serializer = NetCore::make_shared<B>(10);
		manager.AddTask([&serializer]() {
			for (int i = 0; i < 5; ++i)
			{
				auto time = 500 + (i % 2 == 0 ? 1 : -1) * 60;
				serializer->ReserveJob(time, &B::Multifly, 2);
			}
			});
		manager.AddTask([&serializer]() {
			for (int i = 0; i < 5; ++i)
			{
				auto time = 200 + (i % 2 == 0 ? 1 : -1) * 25;
				serializer->ReserveJob(time, &B::Multifly, 3);
			}
			});
		manager.AddTask([&serializer]() {
			auto begin = ::GetTickCount64();
			while (true)
			{
				serializer->Flush();
				if (::GetTickCount64() - begin >= 10000) break;
			}
			});
		manager.JoinAllTasks();
	}

	cout << "end" << endl;
	return 0;
}
