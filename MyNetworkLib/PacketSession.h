#pragma once

NAMESPACE_OPEN(NetCore);

/// <summary>
/// Session for packet (packet which may be controlled by Google::FlatBuffers)
/// </summary>
class PacketSession abstract : public Session
{
	static constexpr uint32 FLUSH_SEND_MIN_INTERVAL_TICK = 1000;
	static constexpr uint32 FLUSH_SEND_MIN_RESERVED_BYTE_LENGTH = 128;
public:
	PacketSession();
	virtual ~PacketSession();

	void SendRaw(const _byte* buffer) override sealed;
	void Send(const uint16 id, _ubyte* ptr, const uint16 size) override sealed;
	void Flush();

protected:
	uint32 OnRecv(const _byte* buffer, const uint32 len) override sealed;
	PURE_VIRTUAL virtual void OnRecvPacket(const _byte* buffer, const uint16 id) = 0;
private:
	_USE_LOCK;
	Vector<WSABUF> _reserved;
	uint32 _reservedSendBytes = 0;
	uint64 _lastSendTick = 0;

};

NAMESPACE_CLOSE;
