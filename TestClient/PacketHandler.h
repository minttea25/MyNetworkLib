#pragma once

#ifdef SERVER
class ClientSession;
using _Session = ClientSession;
#endif // SERVER

#ifdef CLIENT
class ServerSession;
using _Session = ServerSession;
#endif // CLIENT

class PacketHandler
{
public:
	static void TestPacketHandler(const char* pkt, _Session& session);
};

