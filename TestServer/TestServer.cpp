#include "pch.h"

#include <iostream>


constexpr PCSTR IP = "127.0.0.1";
constexpr ushort PORT = 8889;

static void IOCP_WORKER(NetCore::IOCPCore& core)
{
    std::cout << "T id:" << std::this_thread::get_id() << std::endl;
    while (true)
    {
        core.GetQueuedCompletionStatus(200);
        //this_thread::yield();
    }
}

class ClientSession : public NetCore::Session
{
    uint32 OnRecv(const _byte* buffer, const uint32 len) override
    {
        Session::OnRecv(buffer, len);

        std::cout.write(buffer, len) << std::endl;

        return len;
    }
};

int main()
{
    SOCKADDR_IN addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    ::inet_pton(AF_INET, IP, &addr.sin_addr);
    addr.sin_port = ::htons(PORT);
    
    NetCore::IOCPCore core;
    NetCore::Listener listener(addr, []()->NetCore::Session* { return NetCore::xxnew<ClientSession>(); }, core);
    
    if (listener.StartListen() == false) return -1;

    
    std::thread th
    (
        [&core]() {
            IOCP_WORKER(core);
        }
    );
    std::thread th2
    (
        [&core]() {
            IOCP_WORKER(core);
        }
    );

    {
        string msg;
        std::cin >> msg;
        listener.BroadCast(msg.c_str());
    }

    if (th.joinable() == true) th.join();
    if (th2.joinable() == true) th2.join();

    return 0;
}

