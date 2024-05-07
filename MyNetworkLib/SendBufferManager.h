#pragma once

NAMESPACE_OPEN(NetCore);

class SendBufferManager
{
	static constexpr uint32 RESERVE_BUFFER_COUNT = 10;
public:
	SendBufferManager(const SendBufferManager&) = delete;
	SendBufferManager(SendBufferManager&&) noexcept = delete;
	SendBufferManager& operator=(const SendBufferManager&) = delete;
	SendBufferManager& operator=(SendBufferManager&&) = delete;

	SendBufferManager();
	~SendBufferManager();

	void Push(SendBufferSPtr buffer);
	SendBufferSPtr Pop();

	static _ubyte* Reserve(const uint32 size)
	{
		if (size > TLS_SendBuffer->FreeSize())
		{
			// Set new buffer
			TLS_SendBuffer = GSendBufferManager->Pop();
		}
		return TLS_SendBuffer->Reserve(size);
	}

#ifdef DEBUG
	uint16 PoolCount() const { return _poolCount; }
#endif // DEBUG

private:
	inline SendBufferSPtr _get_new()
	{
		return SendBufferSPtr(xxnew<SendBuffer>(), _push);
	}

private: // static
	static inline void _push(SendBuffer* buffer)
	{
		GSendBufferManager->Push(SendBufferSPtr(buffer, _push));
	}
	
private:
	_USE_LOCK;
	Vector<SendBufferSPtr> _buffers;
#ifdef DEBUG
	uint16 _poolCount = 0;
#endif // DEBUG

};

NAMESPACE_CLOSE;

