#pragma once

#ifndef _DEBUG
#pragma comment(lib, "flatbuffers\\Release\\flatbuffers.lib")
#pragma comment(lib, "glog\\Release\\glog.lib")

#else
#pragma comment(lib, "flatbuffers\\Debug\\flatbuffers.lib")
#pragma comment(lib, "glog\\Debug\\glogd.lib")

#endif // !_DEBUG


#include "framework.h"

#include <iostream>
#include <vector>
#include <functional>
#include <future>

#define GLOG_USE_GLOG_EXPORT /*Definition for glog*/
#define GLOG_NO_ABBREVIATED_SEVERITIES /*Definition for glog*/

// glog
#include <glog/logging.h>

// Core 
#include "CoreDef.h"
#include "CoreMacroDef.h"
#include "CoreTypes.h"
#include "CoreTLS.h"
#include "CoreGlobal.h"

#include "CoreSTLContainer.h"

#include "ErrorHandler.h"

#include "CoreLogger.h"

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
#include "MemoryAllocator.h"
#include "MemoryPool.h"
#include "Memory.h"
#include "ObjectPool.h"

#include "Utils.h"
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

// Ex
#include "ClientServiceEx.h"

// Job Headers
#include "Job.h"
#include "JobSerializer.h"
#include "ReservableJob.h"
#include "JobWorker.h"

// flatbuffers lib Headers
#include "flatbuffers/flatbuffers.h"
#include "flatbuffers/util.h"

// flatbuffers Headers
#include "FBAllocator.h"
#include "PacketWrapper.h"

#include "TaskManagerEx.h"


// TEST
#include "Sample.h"