#pragma once

// TEST
#include "Sample.h"

// Base Headers
#include <WinSock2.h>
#include <MSWSock.h>
#include <Windows.h>
#include <WS2tcpip.h>

#include <iostream>
#include <vector>

// Core Headers
#include "CoreDef.h"
#include "CoreMacroDef.h"
#include "CoreTypes.h"
#include "CoreGlobal.h"
#include "CoreTLS.h"

// Network Library
#pragma comment(lib, "ws2_32.lib")


// Memory Headers
#include "Memory.h"
#include "CoreSTLContainer.h"
#include "ObjectPool.h"

// Network Headers
#include "IOCPEvent.h"
#include "IOCPObject.h"
#include "IOCPCore.h"
#include "Connector.h"
#include "Listener.h"
#include "Session.h"
#include "PacketSession.h"




