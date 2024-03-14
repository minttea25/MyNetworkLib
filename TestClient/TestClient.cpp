#include "pch.h"
#include <iostream>

using namespace NetCore;

class TestClass
{
public:
	TestClass(int hp, int mp, int exp) : _hp(hp), _mp(mp), _exp(exp) {}
	~TestClass() { std::cout << "Destructor" << std::endl; }
public:
	static friend std::ostream& operator<<(std::ostream& os, const TestClass& obj)
	{
		return os << "TestClass(" << obj._hp << ',' << obj._mp << ',' << obj._exp << ')';
	}
private:
	int _hp;
	int _mp;
	int _exp;
};

int main()
{
    std::cout << "Hello World!\n";

    Sample sample;
    sample.PrintHello();

	// memory
	TestClass* t = xxnew<TestClass, int, int, int>(100, 50, 0);
	std::cout << *t << std::endl;

	TestClass* t2 = xxnew<TestClass, int, int, int>(100, 50, 0);
	std::cout << *t2 << std::endl;
	xxdelete(t);
	xxdelete(t2);
	
	

    return 0;
}

