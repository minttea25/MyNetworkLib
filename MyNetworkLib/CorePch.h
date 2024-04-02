#pragma once

// TEST
#include "Sample.h"

// Core 
#include "CoreMacroDef.h"
#include "CoreGlobal.h"
#include "CoreDef.h"
#include "CoreTypes.h"
#include "CoreTLS.h"
#include "CoreSTLContainer.h"

#include <iostream>
#include <vector>
#include <functional>

// Base Headers
#include <WinSock2.h>
#include <MSWSock.h>
#include <Windows.h>
#include <WS2tcpip.h>

// Network Library
#pragma comment(lib, "ws2_32.lib")

// Memory Headers
#include "Memory.h"
#include "ObjectPool.h"

#include "ErrorHandler.h"
#include "AddrUtils.h"
#include "SocketUtils.h"

// Network Headers
#include "IOCPEvent.h"
#include "IOCPObject.h"
#include "IOCPCore.h"
#include "Connector.h"
#include "Listener.h"
#include "Session.h"
#include "PacketSession.h"





