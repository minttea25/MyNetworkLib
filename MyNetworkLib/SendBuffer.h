#pragma once

NAMESPACE_OPEN(NetCore);

class SendBuffer;

class SendBufferSegment : public enable_shared_from_this<SendBufferSegment>
{
public:
	SendBufferSegment(SendBufferSPtr buffer, _ubyte* bufferPos, uint32 size)
		: _size(size), _buffer(buffer), _bufferPos(bufferPos)
	{

	}
	~SendBufferSegment()
	{
		DESTRUCTOR(SendBufferSegment);
	}

	_ubyte* GetBufferSegment() const { return _bufferPos; }
	uint32 GetSize() const { return _size; }
private:
	const uint32 _size;
	SendBufferSPtr _buffer;
	_ubyte* _bufferPos;
};

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

	_ubyte* Write(const uint32 size)
	{
		if (size > MAX_BUFFER_SIZE)
		{
			// TODO : Use errorhandler later
			ASSERT_CRASH("The write size is bigger than MAX_BUFFER_SIZE.")
		}
		
		if (size > FreeSize()) _usedSize = 0;

		return &_buffer[_usedSize];
	}
	
	inline uint32 FreeSize() const { return _buffer.size() - _usedSize; }
private:
	Array<_ubyte, MAX_BUFFER_SIZE> _buffer = {};
	uint32 _usedSize = 0;
};

NAMESPACE_CLOSE;

