
#ifndef _DEBUG
pragma comment(lib, "MyNetworkLib\\Release\\MyNetworkLib.lib")

#else
#pragma comment(lib, "MyNetworkLib\\Debug\\MyNetworkLib.lib")

#endif // !_DEBUG

#include "CorePch.h"

#include <iostream>
#include <cmath>

using namespace std;

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
		shared_ptr<A> serializer = NetCore::make_shared<A>(10);
		serializer->PushJob([]() { cout << "task1" << endl; });
		serializer->PushJob(&A::Multifly, 10);
		serializer->PushJob(&A::Multifly, 200);

		while (serializer->IsEmpty() == false)
		{
			serializer->DoOneTask();
		}
	}
	{
		shared_ptr<B> serializer = NetCore::make_shared<B>(20);
		serializer->ReserveJob([]() { cout << "task2" << endl; }, 1000);
		serializer->ReserveJob(1500, &B::Multifly, 10);
		serializer->ReserveJob(200, &B::Multifly, 100);

		serializer->DoWork();
	}

	
	cout << "end" << endl;
	return 0;
}
