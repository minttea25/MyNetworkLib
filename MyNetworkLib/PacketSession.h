#pragma once

NAMESPACE_OPEN(NetCore);

class PacketSession : public Session
{
	enum : uint32
	{
		FLUSH_SEND_MIN_INTERVAL_TICK = 1000,
		FLUSH_SEND_MIN_RESERVED_BYTE_LENGTH = 128,

	};
public:
	PacketSession();
	virtual ~PacketSession();

	void Send(const _byte* buffer) override sealed;
	void Flush();

protected:
	uint32 OnRecv(const _byte* buffer, const uint32 len) override sealed;
	virtual void OnRecvPacket(const _byte* buffer, const int32 len) PURE_VIRTUAL;
private:
	_USE_LOCK;
	Vector<std::shared_ptr<SendBufferSegment>> _reserved;
	uint32 _reservedSendBytes = 0;
	uint64 _lastSendTick = 0;

};

NAMESPACE_CLOSE;
