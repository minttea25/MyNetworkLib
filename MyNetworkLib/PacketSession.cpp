#include "pch.h"
#include "PacketSession.h"

NetCore::PacketSession::PacketSession()
	: Session(), _lastSendTick(GetTickCount64())
{
}

NetCore::PacketSession::~PacketSession()
{
	DESTRUCTOR(PacketSession);
}

void NetCore::PacketSession::Send(const _byte* buffer)
{
	if (buffer == nullptr) return;

	const uint32 size = static_cast<uint32>(strlen(buffer));
	auto pos = TLS_SendBuffer->Write(size);
	std::copy(buffer, buffer + size, pos);

	{
		_WRITE_LOCK;

		_reserved.push_back(NetCore::make_shared<SendBufferSegment>(
			TLS_SendBuffer->shared_from_this(),
			pos,
			size));
		_reservedSendBytes += size;
	}
}

void NetCore::PacketSession::Flush()
{
	// check condition
	uint64 dt = ::GetTickCount64() - _lastSendTick;
	if (dt < FLUSH_SEND_MIN_INTERVAL_TICK
		&& _reservedSendBytes < FLUSH_SEND_MIN_RESERVED_BYTE_LENGTH) return;

	Vector<std::shared_ptr<SendBufferSegment>> list;
	{
		_WRITE_LOCK;
		const size_t size = _reserved.size();
		if (size == 0) return;

		
		// move
		list.reserve(size);
		std::move(_reserved.begin(), _reserved.end(), std::back_inserter(list));

		// clear values
		_reserved.clear();
		_lastSendTick = ::GetTickCount64();
		_reservedSendBytes = 0;
	}

	Session::_send(list);
}

NetCore::uint32 NetCore::PacketSession::OnRecv(const _byte* buffer, const uint32 len)
{
	// TEMP : 한번에 출력중...
	OnRecvPacket(buffer, len);

	return len;
}

