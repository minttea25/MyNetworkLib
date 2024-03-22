#include "pch.h"

#include <iostream>


constexpr PCSTR IP = "127.0.0.1";
constexpr ushort PORT = 8889;

static void IOCP_WORKER(NetCore::IOCPCore& core)
{
    std::cout << "T id:" << std::this_thread::get_id() << std::endl;
    std::this_thread::sleep_for(100ms);
    while (true)
    {
        core.GetQueuedCompletionStatus(100);
        this_thread::yield();
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
    listener.StartListen();

    NetCore::IOCPCore core;
    std::thread th
    (
        [&core]() {
            IOCP_WORKER(core);
        }
    );

    if (th.joinable() == true) th.join();

    return 0;
}

