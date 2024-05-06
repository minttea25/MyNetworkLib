#pragma once

#include "PacketManager.h"

class ServerSession : public NetCore::PacketSession
{
public:
	void OnRecvPacket(const char* buffer, const unsigned __int16 id) override
	{
		GPacketManager->Dispatch(id, buffer, *this);
	}
	virtual void OnDisconnected(const int error) override
	{
		std::cout << "disconnected: " << error << std::endl;
	}
};

