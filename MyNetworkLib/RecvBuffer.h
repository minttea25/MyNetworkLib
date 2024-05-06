#pragma once

NAMESPACE_OPEN(NetCore);

class RecvBuffer
{
	static constexpr uint16 BUFFER_COUNT = 10;
public:
	RecvBuffer(const uint32 bufferSize);
	~RecvBuffer();

	bool Read(const uint32 size);
	bool Write(const uint32 size);
	void Clear();

	_byte* WritePos() { return &_buffer[_writePos]; }
	_byte* DataPos() { return &_buffer[_dataPos]; }

	const uint32 DataSize() const noexcept { return _writePos - _dataPos; }
	const uint32 FreeSize() const noexcept { return _bufferSize - _writePos; }

private:
	Vector<_byte> _buffer;

	const uint32 _bufferSize;
	const uint32 _capacity;

	uint32 _dataPos;
	uint32 _writePos;
};

NAMESPACE_CLOSE;

