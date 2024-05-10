#pragma once

#define SERVER



#ifndef _DEBUG
pragma comment(lib, "MyNetworkLib\\Release\\MyNetworkLib.lib")



#else
#pragma comment(lib, "MyNetworkLib\\Debug\\MyNetworkLib.lib")


#endif // !_DEBUG

#include "CorePch.h"
#include <map>

#include "flatbuffers/flatbuffers.h"
#include "flatbuffers/util.h"
#include "fbs/Packet_generated.h"

#include "ClientSession.h"
#include "SessionManager.h"

#include "PacketHandler.h"
#include "PacketManager.h"
