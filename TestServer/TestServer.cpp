#include "pch.h"

#include <iostream>
#include "flatbuffers/flatbuffers.h";
#include "flatbuffers/util.h"
#include "fbs/Packet_generated.h"

using namespace NetCore;

constexpr PCSTR IP = "127.0.0.1";
constexpr ushort PORT = 8900;

static bool off = false;


class ClientSession : public NetCore::PacketSession
{
    void OnConnected() override
    {
        std::cout << "OnConnected" << endl;
    }

    void OnRecvPacket(const _byte* buffer, const ushort id) override
    {
        auto testPkt = Test::GetTestPacket(buffer);
        cout << "Msg: " << testPkt->msg()->c_str() << '\n';
        cout << "Number: " << testPkt->number() << '\n';
        cout << endl;
    }

    void OnDisconnected(const int32 error) override
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

int main()
{
    mutex m;
    vector<shared_ptr<ClientSession>> sessions;

    SOCKADDR_IN addr = AddrUtils::GetTcpAddress(IP, PORT);

    auto core = NetCore::make_shared<IOCPCore>();
    auto server = NetCore::make_shared<ServerService>(
        core, addr,
        [=, &sessions, &m]() {
            LockGuard lg(m);
            auto s = NetCore::make_shared<ClientSession>();
            sessions.push_back(s);
            return s;
        },
        10, 10);

    if (server->Start() == false)
    {
        return -1;
    }

    NetCore::Thread::TaskManager manager;

    manager.AddTask(
        [=]() {
            std::cout << "T id:" << TLS_Id << std::endl;
            while (true)
            {
                core->ProcessQueuedCompletionStatus(200);

                if (off) break;

                this_thread::yield();
            }
        }, 3);

    manager.AddTask(
        [=, &sessions, &m]() {
            this_thread::sleep_for(200ms);
            std::cout << "flush T id:" << TLS_Id << std::endl;
            while (true)
            {
                LockGuard lg(m);
                for (auto& s : sessions)
                {
                    if (s->IsConnected())
                        s->Flush();
                }

                if (off)
                {
                    sessions.clear();
                    break;
                }

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
                off = true;
                break;
            }
            else
            {
                //server->Broadcast(msg.c_str());
                auto pkt = GetTestPacket(msg);
                server->Broadcast(1, pkt.first, pkt.second);
            }
        }
    }
    this_thread::sleep_for(500ms);

    manager.JoinAllTasks();
    server->Stop();

    std::cout << server.use_count() << std::endl;

    return 0;
}

