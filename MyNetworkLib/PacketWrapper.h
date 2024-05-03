#pragma once

NAMESPACE_OPEN(NetCore);

#pragma pack(push, 2)
struct PacketHeader
{
public:
	PacketHeader(const uint16 id, const uint16 size) : _id(id), _size(size) {}

	uint16 size() const { return _size; }
	uint16 id() const { return _id; }
private:
	uint16 _size;
	uint16 _id;

	// TODO: Check verifyed values. (after reinterpret_cast)
};
#pragma pack(pop)



class PacketWrapper
{
public:
	static WSABUF Serialize(const uint16 id, _ubyte* dataPtr, const uint16 dataSize)
	{
		const uint16 size = sizeof(PacketHeader) + dataSize;

		// Get memory from SendBuffer
		_ubyte* buffer = TLS_SendBuffer->Reserve(size);

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

