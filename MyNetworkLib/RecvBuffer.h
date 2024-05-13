#pragma once

NAMESPACE_OPEN(NetCore);

/// <summary>
/// RecvBuffer for socket io.
/// </summary>
class RecvBuffer
{
public:
	static constexpr uint16 BUFFER_COUNT = 10;


	RecvBuffer(const uint32 bufferSize);
	~RecvBuffer();

	/// <summary>
	/// Move the read pointer by given byte size.
	/// </summary>
	/// <param name="size">byte size to move pointer</param>
	/// <returns>true if successful, false otherwise. (ex - recvbuffer invalid data)</returns>
	bool Read(const uint32 size);
	/// <summary>
	/// Move the write pointer by given byte size.
	/// </summary>
	/// <param name="size">byte size to move pointer</param>
	/// <returns>true if successful, false otherwise. (ex - recvbuffer overflow)</returns>
	bool Write(const uint32 size);
	/// <summary>
	/// Move the read and write pointers if it is possible to move data front of the buffer.
	/// </summary>
	void Clear();

	/// <summary>
	/// Get pointer of the write position.
	/// </summary>
	/// <returns></returns>
	_byte* WritePos() { return &_buffer[_writePos]; }
	/// <summary>
	/// Get pointer of the read position.
	/// </summary>
	/// <returns></returns>
	_byte* DataPos() { return &_buffer[_dataPos]; }

	/// <summary>
	/// Get data size (write position - read position)
	/// </summary>
	/// <returns>written data size</returns>
	const uint32 DataSize() const noexcept { return _writePos - _dataPos; }
	/// <summary>
	/// Get free size (buffer size - write position)
	/// </summary>
	/// <returns>possible writable size</returns>
	const uint32 FreeSize() const noexcept { return _bufferSize - _writePos; }

private:
	Vector<_byte> _buffer;

	const uint32 _bufferSize;
	const uint32 _capacity;

	uint32 _dataPos;
	uint32 _writePos;
};

NAMESPACE_CLOSE;

