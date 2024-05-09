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

void NetCore::PacketSession::SendRaw(const _byte* buffer)
{
	// TEMP : deprecated
	if (buffer == nullptr) return;

	const uint32 size = static_cast<uint32>(strlen(buffer));
	auto pos = TLS_SendBuffer->Reserve(size);
	std::copy(buffer, buffer + size, pos);

	{
		_WRITE_LOCK;
		
		_reservedSendBytes += size;
	}
}

void NetCore::PacketSession::Send(const uint16 id, _ubyte* ptr, const uint16 size)
{
	if (ptr == nullptr) return;

	auto buf = NetCore::PacketWrapper::Serialize(id, ptr, size);
	{
		_WRITE_LOCK;
		_reserved.push_back(buf);

		_reservedSendBytes += (size + sizeof(PacketHeader));
	}
}

void NetCore::PacketSession::Flush()
{
	// check condition
	uint64 dt = ::GetTickCount64() - _lastSendTick;
	if (dt < FLUSH_SEND_MIN_INTERVAL_TICK
		&& _reservedSendBytes < FLUSH_SEND_MIN_RESERVED_BYTE_LENGTH) return;

	Vector<WSABUF> list;
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
	//PRINT(Received Total Len : , len);

	int32 n = 0;
	uint32 processed = 0;

	while (processed < len)
	{
		// 1: size, 2: id
		
		auto ptr = buffer + processed;
		const PacketHeader* header = reinterpret_cast<const PacketHeader*>(ptr);

		const uint16 size = header->size(); // total size of packet
		const uint16 id = header->id();

		//PRINT(Received: , n);
		//PRINT(Size: , size);
		//PRINT(Id: , id);

		OnRecvPacket(ptr + sizeof(PacketHeader), id);

		processed += size;
		n++;
	}

	return processed;
}

