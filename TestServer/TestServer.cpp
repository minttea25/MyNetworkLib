#include "pch.h"

#include <iostream>


constexpr PCSTR IP = "127.0.0.1";
constexpr ushort PORT = 8889;

static void IOCP_WORKER(NetCore::IOCPCore& core)
{
    std::cout << "T id:" << std::this_thread::get_id() << std::endl;
    while (true)
    {
        core.GetQueuedCompletionStatus(10);
        //this_thread::yield();
    }
}

int main()
{
    SOCKADDR_IN addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    ::inet_pton(AF_INET, IP, &addr.sin_addr);
    addr.sin_port = ::htons(PORT);
    

    NetCore::Listener listener(addr);
    NetCore::IOCPCore core;
    listener.StartListen(core);

    
    std::thread th
    (
        [&core]() {
            IOCP_WORKER(core);
        }
    );

    {
        while (true)
        {
            /*auto s = listener.GetSession(0);
            if (s != nullptr)
            {
                std::cout << s->IsConnected() << std::endl;
            }
            this_thread::sleep_for(100ms);*/
            string msg;
            std::cin >> msg;
            auto s = listener.GetSession(0);
            if (s != nullptr)
            {
                s->Send(msg.c_str());
            }
        }
    }

    if (th.joinable() == true) th.join();

    return 0;
}

