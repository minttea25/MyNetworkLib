#pragma once

#include <mutex>
#include <WinSock2.h>

NAMESPACE_OPEN(NetCore);

using _byte = char;
using _ubyte = unsigned char;
using int8 = __int8;
using int16 = __int16;
using int32 = __int32;
using int64 = __int64;
using uint8 = unsigned __int8;
using uint16 = unsigned __int16;
using uint32 = unsigned __int32;
using uint64 = unsigned __int64;
 
using ushort = unsigned __int16;

template<typename T>
using Atomic = std::atomic<T>;
using Mutex = std::mutex;
using CondVar = std::condition_variable;
using UniqueLock = std::unique_lock<std::mutex>;
using LockGuard = std::lock_guard < std::mutex>;

using Socket = SOCKET;

using task_id = uint32;
using count_t = uint32;

#define SHARED_PTR(name) using name##SPtr = std::shared_ptr<class name>;
#define SHARED_PTR_S(name) using name##SPtr = std::shared_ptr<struct name>;
SHARED_PTR(IOCPCore);
SHARED_PTR(IOCPObject);
SHARED_PTR_S(IOCPEvent);
SHARED_PTR(Session);
SHARED_PTR(Connector);
SHARED_PTR(Listener);
SHARED_PTR(Service);
SHARED_PTR(Job);
SHARED_PTR(TimeJob);
SHARED_PTR(SendBuffer);

#define WEAK_PTR(name) using name##WPtr = std::weak_ptr<class name>;

WEAK_PTR(Session);
WEAK_PTR(IOCPObject);

// Lock type
using Lock = class RWLock;

NAMESPACE_CLOSE;