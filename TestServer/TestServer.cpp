#include "pch.h"

using namespace NetCore;

std::wstring IP(L"127.0.0.1");
constexpr ushort PORT = 8900;

static bool off = false;

static pair< uint8_t*, _ubyte> GetTestPacket(const string& msg)
{
    const ushort id = 1; // fixed;

    NetCore::FBAllocator allocator;
    flatbuffers::FlatBufferBuilder builder(1024, &allocator);
    auto msg_os = builder.CreateString(msg);
    auto pkt = Test::CreateTestPacket(builder, msg_os, 100);
    builder.Finish(pkt);

    return { builder.GetBufferPointer(), builder.GetSize() };
}

static void FlushSessionJob()
{
    std::cout << "FlushSessionJob on tid:" << NetCore::TLS_Id << std::endl;
    GSessionManager->FlushSessions();
    if (!off) 
        NetCore::GGlobalJobQueue->PushJob(&FlushSessionJob, 500);
}

constexpr int SESSION_COUNT = 100;
constexpr int BACK_LOG = 100;

int main(int argc, char* argv)
{
    NetCore::LoggerConfig config;
    config.DirPath = "../TestLogs/Server";
    config.ErrorPathPrefix = "ERROR_";
    config.FatalPathPrefix = "FATAL_";
    config.InfoPathPrefix = "INFO_";
    config.WarningPathPrefix = "WARNING_";

    //NetCore::InitNetCore(argv, "../TestLogs/Server");
    NetCore::InitNetCore(argv, &config);

    GPacketManager = new PacketManager();
    GSessionManager = new SessionManager();

    SOCKADDR_IN addr = AddrUtils::GetTcpAddress(IP, PORT);

    auto session_factory = [&]() -> NetCore::SessionSPtr {
        return GSessionManager->SessionFactory();
        };

    auto core = NetCore::make_shared<IOCPCore>();
    auto server = NetCore::make_shared<ServerService>(
        core, addr,
        session_factory,
        SESSION_COUNT, BACK_LOG);

    if (server->Start() == false)
    {
        return -1;
    }

    NetCore::TaskManagerEx manager;

    manager.AddTask(
        [&]() {
            std::cout << "T id:" << TLS_Id << std::endl;
            while (true)
            {
                core->ProcessQueuedCompletionStatusEx(200);

                manager.DoWorkJob(50);
                manager.DoWorkReservedJob(50);

                if (off) break;

                this_thread::yield();
            }
        }, 3);

    FlushSessionJob();

    while (!off)
    {
        {
            string msg;
            std::cin >> msg;

            if (strcmp(msg.c_str(), "stop") == 0)
            {
                off = true;
                break;
            }
            else
            {
                auto pkt = GetTestPacket(msg);
                server->Broadcast(1, pkt.first, pkt.second);
            }
        }
    }
    this_thread::sleep_for(300ms);

    manager.JoinAllTasks();
    server->Stop();

    delete GSessionManager;
    delete GPacketManager;

    return 0;
}

