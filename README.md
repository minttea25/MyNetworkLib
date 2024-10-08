# MyNetworkLib
This library provides functions and utilities related to TCP connections and server operations using Winsock.

## Build Compiler
`MSVC (Microsoft Visual C++) `

## C++ Standard
`ISO C++ 17 Standard` with Visual Studio 2022

## Dependencies
- [Google::FlatBuffers](https://flatbuffers.dev/) (24.3.7)
- [Google::glog](https://github.com/google/glog)
- winsock2 (ws2_32.lib)

## API Documentation & Library Documentation
The documentation is located at [MyNetworkLibDoc](https://github.com/minttea25/MyNetworkLibDoc) generated by [DoxyGen](https://www.doxygen.nl/)


### Usages
#### Base
- `Initialize Library` : Initializes the core components of the library.
    ```cpp
    #include "CorePch.h"
    
    int main(int argc, char* argv) 
    {
    ...
    std::string LOGDIR("../Logs");
    NetCore::InitNetCore(argv, LOGDIR);
    
    // Set values of google::glog after initializing the library
    googld::LogToStderr(); // such this code
    ...
    }
    ...
    ```

#### Services
- `ClientService`: Starts the client service. Requires an IOCPCore object, service configuration settings, and a session factory.
    ```cpp
    #include "CorePch.h"
    
    std::wstring IP(L"127.0.0.1");
    constexpr ushort PORT = 8900;
    
    int main(int argc, char* argv) 
    {
        ...
        SOCKADDR_IN addr = AddrUtils::GetTcpAddress(IP, PORT);
        auto session_factory = [&]() -> std::shared_ptr<NetCore::Session> { return MySession::GetSession(); }
        auto core = NetCore::make_shared<NetCore::IOCPCore>();
        
        auto client = NetCore::make_shared<NetCore::ClientService>
        (
            core, addr, session_factory
        );
        
        if (client->Start() == false) { // error }
    }
    ```
- `ServerService`: Starts the server service, waiting for client connections. Similarly, it requires an IOCP object, configuration settings, and a session factory.
    ```cpp
    #include "CorePch.h"
    
    std::wstring IP(L"127.0.0.1");
    constexpr ushort PORT = 8900;
    
    int main(int argc, char* argv) 
    {
        ...
        SOCKADDR_IN addr = AddrUtils::GetTcpAddress(IP, PORT);
        auto session_factory = [&]() -> std::shared_ptr<NetCore::Session> { return MySession::GetSession(); }
        auto core = NetCore::make_shared<NetCore::IOCPCore>();
        const int sessionCount = 10;
        const int backlog = 50;
        
        auto server = NetCore::make_shared<NetCore::ServerService>
        (
            core, addr, session_factory, sessionCount, backlog
        );
        
        if (server->Start() == false) { // error }
    }
    ```
#### Session
- `Session`: Represents a session connected to a RemoteEndpoint.
    ```cpp
    #include "CorePch.h"
    
    class MySession : NetCore::Session
    {
        ...
        // These are virtual method of Session
        void OnConnected() override;
        void OnSend(const int len) override;
        unsigned int OnRecv(const char* buffer, const int len) override;
        void OnDisconnected(const int error = NetCore::DisconnectError::NONE) override;
    }
    ```
- `PacketSession`: Handles serialized data from `Google::flatbuffers`, parsing and processing the packets. It batches packets based on specific conditions and requires a manual `PacketSession::Flush` call. Similar to Session, you can override virtual functions. However, `Session::OnRecv` is implemented in PacketSession, and you can override `PacketSession::OnRecvPacket` instead.
    ```cpp
    #include "CorePch.h"
    
    class MySession : NetCore::PacketSession
    {
        ...
        // These are virtual method of Session
        void OnConnected() override;
        void OnSend(const int len) override;
        void OnDisconnected(const int error = NetCore::DisconnectError::NONE) override;
        void OnRecvPacket(const char* buffer, const uint16_t id) override;
    };
    ```

#### Threads
- `PushJob`: Adds a `Job` to the serialized queue. If the function pointer to be executed by the `Job` belongs to another class, that class must inherit from `JobSerializer`. The object owning this pointer must be kept in memory and should be of type `std::shared_ptr`. The added Job will either execute immediately or be processed by an appropriate thread later.
    ```cpp
    #include "CorePch.h"
    
    class GameMap : public NetCore::JobSerializer
    {
        void AddPlayer(const unsigned long long id);
    };
    
    int main(int argc, char* argv) 
    {
        ...
        auto map = NetCore::make_shared<GameMap>();
        // Use with std::function
        map->PushJob([&] { std::cout << "Job" << std::endl; });
        
        // Use with GameMap::func
        map->PushJob(&GameMap::AddPlayer, 12345678);
    }
    ```
- `PushReservableJob`: Adds a cancelable `ReservableJob` to the serialized queue. Like `Job`, the function pointer owner must inherit from `JobSerializer`, and it must be a `std::shared_ptr`. The added Job is re-added to the JobQueue based on the reserved tick. If the scheduled task has not been executed yet, it can be canceled using `ReservableJob::Cancel`.
    ```cpp
    #include "CorePch.h"
    
    class GameMap : public NetCore::JobSerializer
    {
        void AddPlayer(const unsigned long long id);
    };
    
    
    int main(int argc, char* argv) 
    {
        ...
        auto map = NetCore::make_shared<GameMap>();
        
        // Call GameMap::AddPlayer method with parameter, 12345678 after 1000 ticks
        auto job = map->PushReservableJob(1000, &GameMap::AddPlayer, 12345678);
        
        // If you want to cancel the reserved job,
        job->Cancel();
    }
    ```
- `TaskManagerEx`: A class that handles IOCP tasks and global job workers.
    ```cpp
    #include "CorePch.h"
    
    int main(int argc, char* argv) 
    {
        ...
        auto core = NetCore::make_shared<NetCore::IOCPCore>();
        
        auto manager = NetCore::make_shared<NetCore::TaskManagerEx>();
        
        manager->AddTask([&]() {
           while (true) 
           {
                core->ProcessQueuedCompletionStatusEx(64UL);
                manager->DoWorkJob();
                manager->CheckReservedJob();
                
                this_thread::yield();
           }
        }, 3);
    }
    ```

### Packets
- This library used `Google::flatbuffers` library for serializing data in default. It creates a packet adding the header with the data serialized by flatbuffers. 
- The packet header follows: `[size, 2 bytes][id, 2 bytes]` with alignment 2.
- Data layout of a packet: `[size, 2][id, 2][data, N]`
- For more serializations, see `PacketWrapper` class.

### Loggings
- It uses `Google::glog` library as logging. Initializing NetCore library contains initializing glog.



---
# MyNetworkLib
이 라이브러리는 winsock을 사용하여 TCP 연결과 관련된 함수와 기능 및 서버 관련 유틸을 제공합니다.

## Build Compiler
`MSVC (Microsoft Visual C++) `

## C++ Standard
`ISO C++ 17 Standard` with Visual Studio 2022

## Dependencies
- [Google::FlatBuffers](https://flatbuffers.dev/) (24.3.7)
- [Google::glog](https://github.com/google/glog)
- winsock2 (ws2_32.lib)

## API Documentation & Library Documentation
The documentation is located at [MyNetworkLibDoc](https://github.com/minttea25/MyNetworkLibDoc) generated by [DoxyGen](https://www.doxygen.nl/)



### Usages
#### Base
- `Initialize Library`
    ```cpp
    #include "CorePch.h"
    
    int main(int argc, char* argv) 
    {
    ...
    std::string LOGDIR("../Logs");
    NetCore::InitNetCore(argv, LOGDIR);
    
    // Set values of google::glog after initializing the library
    googld::LogToStderr(); // such this code
    ...
    }
    ...
    ```

#### Services
- `ClientService`: 클라이언트를 시작합니다. IOCPCore 객체와, 서비스 설정 값, 세션에 대한 팩토리가 필요합니다.
    ```cpp
    #include "CorePch.h"
    
    std::wstring IP(L"127.0.0.1");
    constexpr ushort PORT = 8900;
    
    int main(int argc, char* argv) 
    {
        ...
        SOCKADDR_IN addr = AddrUtils::GetTcpAddress(IP, PORT);
        auto session_factory = [&]() -> std::shared_ptr<NetCore::Session> { return MySession::GetSession(); }
        auto core = NetCore::make_shared<NetCore::IOCPCore>();
        
        auto client = NetCore::make_shared<NetCore::ClientService>
        (
            core, addr, session_factory
        );
        
        if (client->Start() == false) { // error }
    }
    ```
- `ServerService`: 클라이언트 접속을 기다리는 서버 서비스를 시작합니다. 마찬가지로 IOCP 객체와, 설정 값, 세션 팩토리가 필요합니다.
    ```cpp
    #include "CorePch.h"
    
    std::wstring IP(L"127.0.0.1");
    constexpr ushort PORT = 8900;
    
    int main(int argc, char* argv) 
    {
        ...
        SOCKADDR_IN addr = AddrUtils::GetTcpAddress(IP, PORT);
        auto session_factory = [&]() -> std::shared_ptr<NetCore::Session> { return MySession::GetSession(); }
        auto core = NetCore::make_shared<NetCore::IOCPCore>();
        const int sessionCount = 10;
        const int backlog = 50;
        
        auto server = NetCore::make_shared<NetCore::ServerService>
        (
            core, addr, session_factory, sessionCount, backlog
        );
        
        if (server->Start() == false) { // error }
    }
    ```
#### Session
- `Session`: RemoteEndpoint에 연결된 세션입니다.
    ```cpp
    #include "CorePch.h"
    
    class MySession : NetCore::Session
    {
        ...
        // These are virtual method of Session
        void OnConnected() override;
        void OnSend(const int len) override;
        unsigned int OnRecv(const char* buffer, const int len) override;
        void OnDisconnected(const int error = NetCore::DisconnectError::NONE) override;
    }
    ```
- `PacketSession`: `Google::flatbuffers`의 직렬화된 데이터와 해당 패킷을 파싱하여 처리합니다. 패킷을 특정 조건에 따라 모아서 보냅니다. 수동적인 Flush 호출이 필요합니다. `Session`과 마찬가지로 가상 함수를 오버라이드 할 수 있습니다. 단, Session::OnRecv는 `PacketSession`에 구현되어 있고, 대신 PacketSession::OnRecvPacket를 오버라이드 합니다.
    ```cpp
    #include "CorePch.h"
    
    class MySession : NetCore::PacketSession
    {
        ...
        // These are virtual method of Session
        void OnConnected() override;
        void OnSend(const int len) override;
        void OnDisconnected(const int error = NetCore::DisconnectError::NONE) override;
        void OnRecvPacket(const char* buffer, const uint16_t id) override;
    };
    ```

#### Threads
- `PushJob`: 직렬화된 큐에 `Job`을 추가합니다. 만약 `Job`에서 실행 시킬 함수 포인터가 다른 클래스가 가지고 있을 경우, 그 클래스는 `JobSerializer`를 상속하고 있어야 하고, 해당 포인터를 소유하고 있는 객체는 메모리에 유지가 되어야 하며, 그 타입은 `std::shared_ptr`이어야 합니다. 추가된 `Job`은 바로 실행되거나, 이후에 다른 적절한 스레드가 이를 실행 할 것입니다. 다음은 예시 코드입니다.
    ```cpp
    #include "CorePch.h"
    
    class GameMap : public NetCore::JobSerializer
    {
        void AddPlayer(const unsigned long long id);
    };
    
    int main(int argc, char* argv) 
    {
        ...
        auto map = NetCore::make_shared<GameMap>();
        // Use with std::function
        map->PushJob([&] { std::cout << "Job" << std::endl; });
        
        // Use with GameMap::func
        map->PushJob(&GameMap::AddPlayer, 12345678);
    }
    ```
- `PushReservableJob`: 직렬화된 큐에 취소가능한 `ReservableJob`을 추가합니다. `Job`과 마찬가지로, 실행 시킬 함수 포인터의 소유자는 `JobSerializer`를 상속받고 있고, std::shared_ptr여야 합니다. 추가된 `Job`은 예약된 `tick`에 따라 `JobQueue`에 다시 추가됩니다. 만약 예약된 작업이 실행되지 않았다면 `ReservableJob::Cancel`로 취소할 수 있습니다.
    ```cpp
    #include "CorePch.h"
    
    class GameMap : public NetCore::JobSerializer
    {
        void AddPlayer(const unsigned long long id);
    };
    
    
    int main(int argc, char* argv) 
    {
        ...
        auto map = NetCore::make_shared<GameMap>();
        
        // Call GameMap::AddPlayer method with parameter, 12345678 after 1000 ticks
        auto job = map->PushReservableJob(1000, &GameMap::AddPlayer, 12345678);
        
        // If you want to cancel the reserved job,
        job->Cancel();
    }
    ```
- `TaskManagerEx`: IOCP 작업과 `GlobalJobWorker를` 수행하는 클래스입니다. 
    ```cpp
    #include "CorePch.h"
    
    int main(int argc, char* argv) 
    {
        ...
        auto core = NetCore::make_shared<NetCore::IOCPCore>();
        
        auto manager = NetCore::make_shared<NetCore::TaskManagerEx>();
        
        manager->AddTask([&]() {
           while (true) 
           {
                core->ProcessQueuedCompletionStatusEx(64UL);
                manager->DoWorkJob();
                manager->CheckReservedJob();
                
                this_thread::yield();
           }
        }, 3);
    }
    ```

### Packets
- 이 라이브러리는 기본적으로 데이터를 직렬화하기 위해 `Google::flatbuffers` 라이브러리를 사용합니다. flatbuffers에서 직렬화된 데이터의 앞에 헤더를 추가하여 패킷을 만듭니다.
- 패킷 헤더는 다음과 같습니다. `[size, 2 bytes][id, 2 bytes]` (2 바이트 정렬).
- 패킷의 데이터 레이아웃: `[size, 2][id, 2][data, N]`
- `PacketWrapper` 클래스를 참조해주세요.

### Loggings
- 이 라이브러리는 로깅으로 `Google::glog`를 사용하고 있습니다. `Initialize`에 로깅 초기화가 포함되어 있으므로, 이후에 로깅 설정을 해도 됩니다.






