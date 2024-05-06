#include "pch.h"
#include "RecvBuffer.h"

NetCore::RecvBuffer::RecvBuffer(const uint32 bufferSize) 
	: _bufferSize(bufferSize), _capacity(bufferSize* BUFFER_COUNT), _dataPos(0), _writePos(0)
{
	_buffer.resize(_capacity);
}

NetCore::RecvBuffer::~RecvBuffer()
{
}

bool NetCore::RecvBuffer::Read(const uint32 size)
{
	if (size > DataSize()) return false;

	// move data pointer after previous data
	_dataPos += size;
	return true;
}

bool NetCore::RecvBuffer::Write(const uint32 size)
{
	if (size > FreeSize()) return false;
	
	// move write pointer to empty buffer
	_writePos += size;
	return true;
}

void NetCore::RecvBuffer::Clear()
{
	uint32 dataSize = DataSize();
	if (dataSize == 0)
	{
		// reset the position at 0.
		_writePos = _dataPos = 0;
	}
	else if (FreeSize() < _bufferSize)
	{
		// If there is no buffer to use next, 
		// copy the current data to the front 
		// and change the _writePos to the back of the data.
		::memcpy(&_buffer[0], &_buffer[_dataPos], dataSize);
		_dataPos = 0;
		_writePos = dataSize;
	}
}
