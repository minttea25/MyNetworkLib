
#ifndef _DEBUG
pragma comment(lib, "MyNetworkLib\\Release\\MyNetworkLib.lib")

#else
#pragma comment(lib, "MyNetworkLib\\Debug\\MyNetworkLib.lib")

#endif // !_DEBUG

#include "CorePch.h"

#include <iostream>
#include <cmath>

using namespace std;

struct A
{
public:
	A(float aa, float bb, float cc) : a(aa), b(bb), c(cc) {}
	float a;
	float b;
	float c;

	static float calc(A a)
	{
		auto t = sqrt(a.b * a.b - 4 * a.a * a.c);
		return ((-1 * a.b) + t) / 2 * a.a;
	}
};

int main()
{
	auto square = ([](int x) -> int { return x * x; });
	auto func = A(1, 2, 1);
	
	{
		NetCore::Thread::ThreadPool pool(5);
		//pool.enqueue(square, 100);
		//auto t = pool.enqueue(square, 100);
		auto t = pool.enqueue([](int x) {return x * x; }, 100);
		auto t2 = pool.enqueue(square, 10);
		auto t3 = pool.enqueue(A::calc, std::move(func));
		
		cout << t.get() << endl;
		cout << t2.get() << endl;
		cout << t3.get() << endl;

		this_thread::sleep_for(1000ms);
		pool.Stop();
	}
	cout << "end" << endl;
	return 0;
}
