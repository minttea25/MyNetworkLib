#pragma once

NAMESPACE_OPEN(NetCore);

/// <summary>
/// Read-Write lock
/// </summary>
class RWLock
{
	static constexpr inline uint32 FLAG_SIZE = 16;
	static constexpr inline uint32 TIMEOUT_TICK = 10000;
	static constexpr inline uint32 MAX_SPIN_COUNT = 10000;

	enum : uint32
	{
		WRITE_THREAD_MASK = 0xFFFF'0000,
		READ_COUNT_MASK = 0x0000'FFFF,
		EMPTY = 0x0000'0000,
	};
public:
	void WriteLock();
	void WriteUnlock();
	void ReadLock();
	void ReadUnlock();
private:
	inline uint32 _get_lock_thread_id() const
	{
		return (_lockFlag.load() & WRITE_THREAD_MASK) >> FLAG_SIZE;
	}
	inline uint32 _get_write_desired_flag() const
	{
		return (TLS_Id << FLAG_SIZE) & WRITE_THREAD_MASK;
	}
	inline uint32 _get_read_lock_count() const
	{
		return _lockFlag.load() & READ_COUNT_MASK;
	}
private:
	/// <summary>
	/// This flag has information of thread id of writing and count of reading.
	/// <para>FLAG: [WWWW'RRRR]</para>
	/// <para>WWWW (2 bytes) : thread id which has write-lock</para>
	/// <para>RRRR (2 bytes) : thread id which has read-count</para>
	/// </summary>
	Atomic<uint32> _lockFlag = EMPTY;
	/// <summary>
	/// Same thread can get multiple lock on writing. 
	/// <para>Note: This variable must be accessed when the thread is same as thread the write flag has.</para>
	/// </summary>
	uint16 _writeCount = 0;
};

NAMESPACE_CLOSE;

