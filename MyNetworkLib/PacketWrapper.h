#pragma once

NAMESPACE_OPEN(NetCore);

struct PacketHeader
{
public:
	PacketHeader(const ushort id, const ushort size) : _id(id), _size(size) {}
private:
	const ushort _id;
	const ushort _size;
};

class PacketBody
{
public:
};



class PacketWrapper
{
public:
	/*template<typename Data>
	[[nodiscard]]
	static std::shared_ptr<Data> Serialize(_ubyte* ptr, const uint32 size)
	{
		const Data* root = flatbuffers::GetRoot<Data>(ptr);

	}*/

	// NEED TO BE CHECKED
	std::pair<WSABUF, WSABUF> Serialize(const ushort id,_ubyte* dataPtr, const ushort len)
	{
		PacketHeader h(id, len);
		WSABUF header{};
		header.buf = reinterpret_cast<char*>(&h);
		header.len = sizeof(PacketHeader);

		WSABUF body{};
		body.buf = reinterpret_cast<char*>(dataPtr);
		body.len = len;


		return { header, body };
	}

};

NAMESPACE_CLOSE;

