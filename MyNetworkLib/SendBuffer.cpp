#include "pch.h"
#include "SendBuffer.h"

NetCore::SendBuffer::SendBuffer()
{
}

NetCore::SendBuffer::~SendBuffer()
{
	DESTRUCTOR(SendBuffer);
}


NetCore::_ubyte* NetCore::SendBuffer::Reserve(const uint32 size)
{
	if (size > MAX_BUFFER_SIZE)
	{
		ErrorHandler::AssertCrash(Errors::APP_SENDBUFFER_SIZE_OVERFLOW);
	}

	// if (size > FreeSize()) return nullptr; <- not occurs

	::memset(&_buffer[_usedSize], 0, size);

	uint32 ret = _usedSize;
	_usedSize += size;

	return &_buffer[ret];
}
