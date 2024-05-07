#include "pch.h"
#include "SendBufferManager.h"

NetCore::SendBufferManager::SendBufferManager()
{
	_buffers.reserve(RESERVE_BUFFER_COUNT);
}

NetCore::SendBufferManager::~SendBufferManager()
{
	_buffers.clear();
}

void NetCore::SendBufferManager::Push(SendBufferSPtr buffer)
{
	_WRITE_LOCK;
	_buffers.push_back(buffer);
}

NetCore::SendBufferSPtr NetCore::SendBufferManager::Pop()
{
	{
		_WRITE_LOCK;

		if (_buffers.empty() == false)
		{
#ifdef DEBUG
			_poolCount++;
			if (_poolCount > RESERVE_BUFFER_COUNT)
			{
				WARN(Poolcount in SendBufferManager exceeds RESERVE_BUFFER_COUNT.);
			}
#endif // DEBUG
			SendBufferSPtr buffer = _buffers.back();
			_buffers.pop_back();
			return buffer;
		}
	}

	// make new
	return SendBufferSPtr(xxnew<SendBuffer>(), _push);
}
