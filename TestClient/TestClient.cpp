#include "pch.h"
#include <iostream>

using namespace NetCore;

static bool off = false;

class TestClass : public std::enable_shared_from_this<TestClass>
{
public:
	TestClass() { std::cout << "Constructor: " << _id << std::endl; }
	TestClass(uint32 id, int hp, int exp) : _id(id), _hp(hp), _exp(exp) {}
	~TestClass() { std::cout << "Destructor: " << _id << std::endl; }
public:
	void SetHpExp(const int hp, const int exp)
	{
		_hp = hp;
		_exp = exp;
	}
	static friend std::ostream& operator<<(std::ostream& os, const TestClass& obj)
	{
		return os << "TestClass_" << obj._id << '(' << obj._hp << ',' << obj._exp << ')';
	}
private:
	uint32 _id = 0;
	int _hp = 0;
	int _exp = 0;
};

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
	
	//SocketUtils::Init(); // temp

	SOCKADDR_IN addr = AddrUtils::GetTcpAddress(IP, PORT);

	this_thread::sleep_for(300ms);


	auto core = NetCore::make_shared<IOCPCore>();
	std::shared_ptr<ServerSession> session_ptr = nullptr;
	
	auto session_factory = [&]() -> SessionSPtr
		{
			auto s = NetCore::make_shared<ServerSession>();
			session_ptr = s;
			return s;
		};

	auto client = NetCore::make_shared<ClientService>
		(
			core, addr, session_factory
		);

	if (client->Start() == false)
	{
		return -1;
	}

	NetCore::Thread::TaskManager manager;
	manager.AddTask(
		[=]() {
			std::cout << "T id:" << TLS_Id << std::endl;
			this_thread::sleep_for(100ms);
			while (true)
			{
				core->ProcessQueuedCompletionStatus(200);

				if (off) break;
			}
		});

	while (!off)
	{
		{
			string msg;
			std::cin >> msg;
			if (strcmp(msg.c_str(), "stop") == 0)
			{
				if (session_ptr != nullptr) session_ptr->Disconnect();
				off = true;
			}
			else
			{
				if (session_ptr != nullptr)
				{
					session_ptr->Send(msg.c_str());
				}
			}

			this_thread::sleep_for(500ms);
		}
	}

	manager.JoinAllTasks();

	session_ptr = nullptr;

	client->Stop();

	std::cout << session_ptr.use_count() << std::endl;
	std::cout << client.use_count() << std::endl;
    return 0;
}

