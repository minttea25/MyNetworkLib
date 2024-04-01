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
	SocketUtils::Init(); // temp

	SOCKADDR_IN addr = AddrUtils::GetTcpAddress(IP, PORT);

	this_thread::sleep_for(200ms);


	auto core = std::make_shared<IOCPCore>();
	std::shared_ptr<ServerSession> session_ptr = nullptr;
	
	auto session_factory = [&]() -> SessionSPtr
		{
			auto s = std::make_shared<ServerSession>();
			session_ptr = s;
			return s;
		};

	
	auto connector = std::make_shared<Connector>(core, addr, session_factory);
	
	if (!connector->Connect())
	{
		std::cerr << "Failed to connect." << std::endl;
		return -1;
	}
	
	std::thread th
	(
		[=]() {
			//std::cout << "T id:" << std::this_thread::get_id() << std::endl;
			std::this_thread::sleep_for(100ms);
			while (true)
			{
				core->ProcessQueuedCompletionStatus(1000);
				//this_thread::yield();
				
				if (session_ptr->IsConnected() == false) break;
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

		this_thread::sleep_for(1000ms);
		session_ptr->Disconnect();
	}

	if (th.joinable()) th.join();
	session_ptr = nullptr;
	std::cout << session_ptr.use_count() << std::endl;
	std::cout << connector.use_count() << std::endl;
    return 0;
}

