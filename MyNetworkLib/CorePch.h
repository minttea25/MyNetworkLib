#pragma once

#ifndef _DEBUG
#pragma comment(lib, "flatbuffers\\Release\\flatbuffers.lib")

#else
#pragma comment(lib, "flatbuffers\\Debug\\flatbuffers.lib")

#endif // !_DEBUG

// TEST
#include "Sample.h"

// Core 
#include "CoreDef.h"
#include "CoreMacroDef.h"
#include "CoreTypes.h"
#include "CoreGlobal.h"
#include "CoreTLS.h"
#include "CoreSTLContainer.h"

#include <iostream>
#include <vector>
#include <functional>
#include <future>

// Base Headers
#include <WinSock2.h>
#include <MSWSock.h>
#include <Windows.h>
#include <WS2tcpip.h>

// Network Library
#pragma comment(lib, "ws2_32.lib")

// Thread Headers
#include "TaskManager.h"
#include "RWLock.h"
#include "Lock.h"
#include "ThreadPool.h"
#include "LockQueue.h"
#include "LockStack.h"
#include "LockPriorityQueue.h"

// Memory Headers
#include "Memory.h"
#include "ObjectPool.h"

#include "ErrorHandler.h"
#include "AddrUtils.h"
#include "SocketUtils.h"

// Network Buffers
#include "SendBuffer.h"
#include "RecvBuffer.h"
#include "SendBufferManager.h"

// Network Headers
#include "IOCPEvent.h"
#include "IOCPObject.h"
#include "IOCPCore.h"
#include "Connector.h"
#include "Listener.h"
#include "Session.h"
#include "PacketSession.h"
#include "Service.h"

// Job Headers
#include "Job.h"
#include "JobSerializer.h"
#include "TimeJob.h"
#include "JobSerializerWithTimer.h"

// flatbuffers lib Headers
#include "flatbuffers/flatbuffers.h"
#include "flatbuffers/util.h"

// flatbuffers Headers
#include "FBAllocator.h"
#include "PacketWrapper.h"
