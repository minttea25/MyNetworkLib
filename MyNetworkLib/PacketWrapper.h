#pragma once

NAMESPACE_OPEN(NetCore);

#pragma pack(push, 2)
/// <summary>
/// It contains the size and the id of serialized data. Used for parsing the serialized packet.
/// <para>Alignment is `2 bytes`.</para>
/// </summary>
struct PacketHeader
{
public:
	PacketHeader(const uint16 id, const uint16 size) : _id(id), _size(size) {}

	uint16 size() const { return _size; }
	uint16 id() const { return _id; }
private:
	uint16 _size;
	uint16 _id;
};
#pragma pack(pop)


/// <summary>
/// Use for wrapping serialized data.
/// </summary>
class PacketWrapper
{
public:
	/// <summary>
	/// Wrap the serialized data with given packet id, pointer of serialized data and its size.
	/// </summary>
	/// <param name="id">packet id</param>
	/// <param name="dataPtr">pointer of the serialized data</param>
	/// <param name="dataSize">size of the serialized data</param>
	/// <returns></returns>
	static WSABUF Serialize(const uint16 id, _ubyte* dataPtr, const uint16 dataSize)
	{
		const uint16 size = sizeof(PacketHeader) + dataSize;

		// Get memory from SendBuffer
		// _ubyte* buffer = TLS_SendBuffer->Reserve(size);
		_ubyte* buffer = SendBufferManager::Reserve(size);

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

