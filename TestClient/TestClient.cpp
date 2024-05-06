#include "pch.h"


using namespace NetCore;

static bool off = false;

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
	GPacketManager = new PacketManager();

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
					auto pkt = GetTestPacket(msg);
					session_ptr->Send(1, pkt.first, pkt.second);
				}
			}

			this_thread::sleep_for(500ms);
		}
	}

	manager.JoinAllTasks();

	session_ptr = nullptr;

	client->Stop();

	delete GPacketManager;

	return 0;
}

