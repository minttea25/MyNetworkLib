#include "pch.h"
#include <iostream>

using namespace NetCore;

class TestClass
{
public:
	TestClass() { std::cout << "Constructor: " << _id << std::endl; }
	TestClass(uint32 id, int hp, int exp) : _id(id), _hp(hp), _exp(exp) {}
	~TestClass() { std::cout << "Destructor: " << _id << std::endl; }
public:
	static friend std::ostream& operator<<(std::ostream& os, const TestClass& obj)
	{
		return os << "TestClass_" << obj._id << '(' << obj._hp << ',' << obj._exp << ')';
	}
private:
	uint32 _id = 0;
	int _hp = 0;
	int _exp = 0;
};

int main()
{
    std::cout << "Hello World!\n";

    Sample sample;
    sample.PrintHello();

	Vector<TestClass> v(100);
	SHOW(v.size(): , v.size());

	// object pool test codes
	// Check memory usage
	ObjectPool<TestClass> op;

	auto t1 = op.Acquire(100, 10, 50);
	auto t2 = op.Acquire(200, 10, 50);
	op.Release(t1);
	op.Release(t2);
	SHOW(poolcount, op.poolCount());
	SHOW(usecount, op.useCount());
	for (int i = 0; i<1000; ++i)
	{
		auto t1 = op.Acquire(100, 10, 50);
		auto t2 = op.Acquire(200, 10, 50);

		SHOW(poolcount, op.poolCount());
		SHOW(usecount, op.useCount());

		op.Release(t1);
		op.Release(t2);

		MESSAGE(After release t1 and t2 : );
		SHOW(poolcount, op.poolCount());
		SHOW(usecount, op.useCount());

		this_thread::sleep_for(10ms);
	}

    return 0;
}

