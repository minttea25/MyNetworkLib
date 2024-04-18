#include "pch.h"

#include <iostream>

using namespace NetCore;

constexpr PCSTR IP = "127.0.0.1";
constexpr ushort PORT = 8900;

static bool off = false;


class ClientSession : public NetCore::Session
{
    uint32 OnRecv(const _byte* buffer, const uint32 len) override
    {
        Session::OnRecv(buffer, len);

        std::cout.write(buffer, len) << std::endl;

        return len;
    }

    virtual void OnDisconnected(const int32 error) override
    {
        std::cout << "disconnected: " << error << std::endl;
    }
};

int main()
{
    SOCKADDR_IN addr = AddrUtils::GetTcpAddress(IP, PORT);
    
    auto core = NetCore::make_shared<IOCPCore>();
    auto server = NetCore::make_shared<ServerService>(
        core, addr,
        NetCore::make_shared<ClientSession>,
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
            }
        }, 5);

    while (true)
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
                server->Broadcast(msg.c_str());
            }
        }
    }
    this_thread::sleep_for(500ms);

    manager.JoinAllTasks();
    server->Stop();

    std::cout << server.use_count() << std::endl;

    return 0;
}

