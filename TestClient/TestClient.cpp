#include "pch.h"
#include <iostream>
#include "flatbuffers/flatbuffers.h";
#include "flatbuffers/util.h"
#include "fbs/Packet_generated.h"

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

class ServerSession : public NetCore::PacketSession
{
public:
	void OnRecvPacket(const _byte* buffer, const ushort id) override
	{
		auto testPkt = Test::GetTestPacket(buffer);
		cout << "Msg: " << testPkt->msg()->c_str() << '\n';
		cout << "Number: " << testPkt->number() << '\n';
		cout << endl;
	}
	virtual void OnDisconnected(const int32 error) override
	{
		std::cout << "disconnected: " << error << std::endl;
	}
};

static pair< uint8_t*, ushort> GetTestPacket(const string& msg)
{
	const ushort id = 1; // fixed;

	NetCore::FBAllocator allocator;
	flatbuffers::FlatBufferBuilder builder(128, &allocator);
	auto msg_os = builder.CreateString(msg);
	auto pkt = Test::CreateTestPacket(builder, msg_os, 100);
	builder.Finish(pkt);

	return { builder.GetBufferPointer(), builder.GetSize() };
}

constexpr PCSTR IP = "127.0.0.1";
constexpr ushort PORT = 8900;

int main()
{

	//SocketUtils::Init(); // temp

	SOCKADDR_IN addr = AddrUtils::GetTcpAddress(IP, PORT);

	this_thread::sleep_for(300ms);


	auto core = NetCore::make_shared<IOCPCore>();
	std::shared_ptr<ServerSession> session_ptr = nullptr;

	auto session_factory = [&]() -> SessionSPtr {
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
			std::cout << "IOCP T id:" << TLS_Id << std::endl;
			this_thread::sleep_for(100ms);
			while (true)
			{
				core->ProcessQueuedCompletionStatus(200);

				if (off) break;

				this_thread::yield();
			}
		});
	manager.AddTask(
		[&]() {
			std::cout << "Session Flush T id:" << TLS_Id << std::endl;
			this_thread::sleep_for(200ms);
			while (true)
			{
				if (session_ptr != nullptr && session_ptr->IsConnected())
				{
					session_ptr->Flush();
				}

				if (off) break;

				this_thread::yield();
			}
		}
	);

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
					//session_ptr->Send(msg.c_str());
					auto pkt = GetTestPacket(msg);
					session_ptr->Send_(1, pkt.first, pkt.second);
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

