#pragma once

NAMESPACE_OPEN(NetCore);

class SendBuffer : public enable_shared_from_this<SendBuffer>
{
public:
	static constexpr uint32 MAX_BUFFER_SIZE = 0x1000; // 4096 bytes
	//static constexpr uint32 MAX_BUFFER_SIZE = 0x100; // TEST

	SendBuffer();
	~SendBuffer();

	_ubyte* Reserve(const uint32 size);

	inline uint32 FreeSize() const { return _buffer.size() - _usedSize; }

private:
	Array<_ubyte, MAX_BUFFER_SIZE> _buffer = {};
	uint32 _usedSize = 0;

};

NAMESPACE_CLOSE;

