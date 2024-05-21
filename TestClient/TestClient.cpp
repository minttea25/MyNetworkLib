#include "pch.h"


using namespace NetCore;

static bool off = false;

static pair< uint8_t*, ushort> GetTestPacket(const string& msg)
{
	const ushort id = 1; // fixed;

	NetCore::FBAllocator allocator;
	flatbuffers::FlatBufferBuilder builder(1024, &allocator);
	auto msg_os = builder.CreateString(msg);
	auto pkt = Test::CreateTestPacket(builder, msg_os, 100);
	builder.Finish(pkt);

	return { builder.GetBufferPointer(), builder.GetSize() };
}

std::wstring IP(L"127.0.0.1");
constexpr ushort PORT = 8900;

constexpr int SESSION_COUNT = 10;

int main(int argc, char* argv)
{
	NetCore::InitNetCore(argv, "../TestLogs/Client");

	GPacketManager = new PacketManager();
	GSessionManager = new SessionManager();

	SOCKADDR_IN addr = AddrUtils::GetTcpAddress(IP, PORT);

	this_thread::sleep_for(300ms);

	auto session_factory = [&]() -> NetCore::SessionSPtr {
		return GSessionManager->SessionFactory();
		};

	auto core = NetCore::make_shared<IOCPCore>();
	auto client = NetCore::make_shared<ClientServiceEx>
		(
			core, addr, session_factory, SESSION_COUNT
		);

	if (client->Start() == false)
	{
		return -1;
	}

	NetCore::Thread::TaskManager manager;
	manager.AddTask(
		[&]() {
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
				GSessionManager->FlushSessions();

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
				client->DisconnectAll();
				off = true;
			}
			else
			{
				auto pkt = GetTestPacket(msg);
				client->Send(1, pkt.first, pkt.second);
			}

			this_thread::sleep_for(100ms);
		}
	}

	this_thread::sleep_for(3000ms);

	manager.JoinAllTasks();

	client->Stop();

	delete GPacketManager;
	delete GSessionManager;

	return 0;
}

