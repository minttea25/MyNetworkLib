#pragma once

NAMESPACE_OPEN(NetCore);

/// <summary>
/// Each thread has Sendbuffer.
/// </summary>
class SendBuffer : public enable_shared_from_this<SendBuffer>
{
public:
	static constexpr uint32 MAX_BUFFER_SIZE = 0x1000; // 4096 bytes
	//static constexpr uint32 MAX_BUFFER_SIZE = 0x100; // TEST

	SendBuffer();
	~SendBuffer();

	/// <summary>
	/// Move the pointer by the byte size and get the pointer of the writable buffer.
	/// </summary>
	/// <param name="size">byte size to reserve</param>
	/// <returns>writable pointer of the buffer</returns>
	_ubyte* Reserve(const uint32 size);

#pragma warning(disable: 4267)
	/// <summary>
	/// Get free size of the buffer
	/// </summary>
	/// <returns>free size</returns>
	inline uint32 FreeSize() const { return _buffer.size() - _usedSize; }
#pragma warning(default: 4267)

private:
	Array<_ubyte, MAX_BUFFER_SIZE> _buffer = {};
	uint32 _usedSize = 0;

};

NAMESPACE_CLOSE;

