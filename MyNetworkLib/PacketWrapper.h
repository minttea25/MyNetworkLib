#pragma once

NAMESPACE_OPEN(NetCore);

#pragma pack(push, 2)
struct PacketHeader
{
public:
	PacketHeader(const ushort id, const ushort size) : _id(id), _size(size) {}

	ushort size() const { return _size; }
	ushort id() const { return _id; }
private:
	ushort _size;
	ushort _id;

	// TODO: Check verifyed values. (after reinterpret_cast)
};
#pragma pack(pop)



class PacketWrapper
{
public:
	static WSABUF Serialize(const ushort id, _ubyte* dataPtr, const ushort dataSize)
	{
		const uint16 size = sizeof(PacketHeader) + dataSize;

		// Get memory from SendBuffer
		_ubyte* buffer = TLS_SendBuffer->Write(size);

		// Allocate PacketHeader at buffer.
		new(buffer)PacketHeader(id, size);

		// Copy serialized data after PacketHeader
		::memcpy(buffer + sizeof(PacketHeader), dataPtr, dataSize);

		WSABUF buf // len, buf
		{
			size,
			reinterpret_cast<char*>(buffer)
		};
		return buf;
	}
};

NAMESPACE_CLOSE;

