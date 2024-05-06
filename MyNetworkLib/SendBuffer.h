#pragma once

NAMESPACE_OPEN(NetCore);

class SendBuffer : public enable_shared_from_this<SendBuffer>
{
public:
	static constexpr uint32 MAX_BUFFER_SIZE = 0xFFF;

	SendBuffer()
	{

	}
	~SendBuffer()
	{
		DESTRUCTOR(SendBuffer);
	}

	_ubyte* Reserve(const uint32 size)
	{
		if (size > MAX_BUFFER_SIZE)
		{
			// TODO : Use errorhandler later
			ASSERT_CRASH("The write size is bigger than MAX_BUFFER_SIZE.")
		}

		// TEMP
		if (size > FreeSize()) _usedSize = 0;

		::memset(&_buffer[_usedSize], 0, size);

		uint32 ret = _usedSize;
		_usedSize += size;

		return &_buffer[ret];
	}

	inline uint32 FreeSize() const { return _buffer.size() - _usedSize; }
private:
	Array<_ubyte, MAX_BUFFER_SIZE> _buffer = {};
	uint32 _usedSize = 0;
};

NAMESPACE_CLOSE;

