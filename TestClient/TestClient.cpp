#include "pch.h"
#include <iostream>

using namespace NetCore;

//class TestClass
//{
//public:
//	TestClass() { std::cout << "Constructor: " << _id << std::endl; }
//	TestClass(uint32 id, int hp, int exp) : _id(id), _hp(hp), _exp(exp) {}
//	~TestClass() { std::cout << "Destructor: " << _id << std::endl; }
//public:
//	static friend std::ostream& operator<<(std::ostream& os, const TestClass& obj)
//	{
//		return os << "TestClass_" << obj._id << '(' << obj._hp << ',' << obj._exp << ')';
//	}
//private:
//	uint32 _id = 0;
//	int _hp = 0;
//	int _exp = 0;
//};

class ServerSession : public NetCore::Session
{
public:
	uint32 OnRecv(const _byte* buffer, const uint32 len) override
	{
		Session::OnRecv(buffer, len);

		std::cout.write(buffer, len) << std::endl;

		return len;
	}
};

constexpr PCSTR IP = "127.0.0.1";
constexpr ushort PORT = 8900;

int main()
{
	SocketUtils::Init();

	SOCKADDR_IN addr;
	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	::inet_pton(AF_INET, IP, &addr.sin_addr);
	addr.sin_port = ::htons(PORT);

	//this_thread::sleep_for(500ms);


	NetCore::IOCPCore core;
	std::shared_ptr<ServerSession> session_ptr = nullptr;
	
	auto session_factory = [&session_ptr]() -> SessionSPtr
		{
			session_ptr = std::make_shared<ServerSession>();
			return session_ptr;
		};

	
	auto connector = std::make_shared<Connector>(&core, addr, session_factory);
	
	if (!connector->Connect())
	{
		std::cerr << "Failed to connect." << std::endl;
		return -1;
	}
	
	std::thread th
	(
		[&core]() {
			//std::cout << "T id:" << std::this_thread::get_id() << std::endl;
			std::this_thread::sleep_for(100ms);
			while (true)
			{
				core.ProcessQueuedCompletionStatus();
				//this_thread::yield();
			}
		}
	);
	std::thread th2
	(
		[&core]() {
			//std::cout << "T id:" << std::this_thread::get_id() << std::endl;
			std::this_thread::sleep_for(100ms);
			while (true)
			{
				core.ProcessQueuedCompletionStatus();
				//this_thread::yield();
			}
		}
	);

	{
		string msg;
		std::cin >> msg;
		if (session_ptr != nullptr)
		{
			session_ptr->Send(msg.c_str());
		}
	}

	if (th.joinable()) th.join();
	if (th2.joinable()) th2.join();

    return 0;
}

