#pragma once

NAMESPACE_OPEN(NetCore);

class PacketSession : public Session
{
public:
	PacketSession();
	virtual ~PacketSession();

protected:
	virtual void OnRecv(const _ubyte* buffer, const int32 len) sealed;
	virtual void OnRecvPacket(const _ubyte* buffer, const int32 len) abstract;
};

NAMESPACE_CLOSE;
