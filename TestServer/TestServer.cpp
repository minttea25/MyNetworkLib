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

class _JobQueue : public NetCore::GlobalJobSerializer
{
public:
    _JobQueue(int n) : _n(n) {}

    void InitPush()
    {
        PushJob(&_JobQueue::FlushJobQueue);
    }

    std::shared_ptr<NetCore::GlobalJobSerializer> SharedFromThis()
    {
        return shared_from_this();
    }

    void FlushJobQueue()
    {
        std::cerr <<  '[' << _n << "] _JobQueue on tid:" << NetCore::TLS_Id << '\n';
    }

public:
    int _n;
};


class _TimeJobQueue : public NetCore::GlobalTimeJobSerializer
{
public:
    _TimeJobQueue(int n) : _n(n) {}

    void InitReserve()
    {
        ReserveJob(500, &_TimeJobQueue::FlushSessionJob);
    }

    std::shared_ptr<NetCore::GlobalTimeJobSerializer> SharedFromThis()
    {
        return shared_from_this();
    }

    void FlushSessionJob()
    {
        std::cerr << '[' << _n << "] _TimeJobQueue on tid:" << NetCore::TLS_Id << '\n';
        GSessionManager->FlushSessions();
        if (!off)
            ReserveJob(_n, &_TimeJobQueue::FlushSessionJob);
    }

public:
    int _n;
};

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
    GSessionManager = NetCore::make_shared<SessionManager>();

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

                manager.DoWorkJob();
                manager.DoWorkReservedJob(100);

                if (off) break;

                this_thread::yield();
            }
        }, 3);

    std::shared_ptr<_TimeJobQueue> tq1 = NetCore::make_shared<_TimeJobQueue>(150);
    std::shared_ptr<_TimeJobQueue> tq2 = NetCore::make_shared<_TimeJobQueue>(200);
    std::shared_ptr<_TimeJobQueue> tq3 = NetCore::make_shared<_TimeJobQueue>(250);
    tq1->InitReserve();
    tq2->InitReserve();
    tq3->InitReserve();

    std::shared_ptr<_JobQueue> q1 = NetCore::make_shared<_JobQueue>(10000);
    std::shared_ptr<_JobQueue> q2 = NetCore::make_shared<_JobQueue>(20000);
    std::shared_ptr<_JobQueue> q3 = NetCore::make_shared<_JobQueue>(30000);

    GSessionManager->ReserveJob(100, &SessionManager::FlushSessions);

    manager.AddTask([&]() {
        while (true)
        {
            if (off) return;

            q1->InitPush();
            q2->InitPush();
            q3->InitPush();

            this_thread::sleep_for(50ms);
        }

        });

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

    delete GPacketManager;

    return 0;
}

