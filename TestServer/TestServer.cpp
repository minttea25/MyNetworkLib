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
    
    auto core = std::make_shared<IOCPCore>();
    auto listener = std::make_shared<Listener>(
        addr, []()->NetCore::SessionSPtr { return std::make_shared<ClientSession>(); },
        core);
    
    if (listener->StartListen(10) == false)
    {
        return -1;
    }

    std::thread th
    (
        [=]() {
            std::cout << "T id:" << std::this_thread::get_id() << std::endl;
            while (true)
            {
                core->ProcessQueuedCompletionStatus(200);
                //this_thread::yield();

                if (off)
                    break;
            }
        }
    );

    {
        string msg;
        std::cin >> msg;
        listener->BroadCast(msg.c_str());
        this_thread::sleep_for(1000ms);
        off = true;
        listener->ReleaseAllSessions();
    }

    if (th.joinable() == true) th.join();

    std::cout << listener.use_count() << std::endl;

    return 0;
}

