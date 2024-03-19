#pragma once

NAMESPACE_OPEN(NetCore)

struct MemoryHeader;

// Note: Memory Pool uses single-linked-list of SLIST.

class MemoryPool
{
public:
	MemoryPool(int32 size);
	~MemoryPool();

	/// <summary>
	/// Releases the header to the pool.
	/// </summary>
	/// <param name="ptr">The pointer of the memory header.</param>
	void Release(MemoryHeader* ptr);
	/// <summary>
	/// Acquires a memory header from the pool.
	/// </summary>
	/// <returns>The pointer of the acquired memory header.</returns>
	MemoryHeader* Acquire();

	
	/// <summary>
	/// [Atomic Operation] Get count of elements in use outside of the pool.
	/// </summary>
	/// <returns></returns>
	uint32 useCount() const { return _useCount.load(); }

	/// <summary>
	/// [Atomic Operation] Get count of the elements in the pool.
	/// </summary>
	/// <returns>The count of the elements in the pool.</returns>
	uint32 poolCount() const { return _poolCount.load(); }
#ifdef TEST
	static friend std::ostream& operator<<(std::ostream& os, const MemoryPool* ptr)
	{
		return os << "MemoryPool(" << "_allocSize: " << ptr->_allocSize 
			<< ',' << "_useCount: " << ptr->useCount() 
			<< ',' << "_poolCount:" << ptr->poolCount() << ")";
	}
#endif // TEST

private:
	// Note: Header must come first! (than other members)
	SLIST_HEADER _header;
	int32 _allocSize = 0;

	/// <summary>
	/// The count in use.
	/// </summary>
	Atomic<uint32> _useCount = 0;
	// The count of the elements in the pool now.
	Atomic<uint32> _poolCount = 0;
};

/*******************************************
* MemoryHeader
********************************************/

// Note
// SLIST_ENTRY is in windows.h (winapi)
// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/ns-wdm-_slist_entry
// why SLIST_ENTRY?
// for multi-threading enviornment
// On 64-bit platforms, SLIST_ENTRY structures must be 16-byte aligned.
// MEMORY_ALLOCATION_ALIGNMENT is 16 at 64-bit platforms.

// MemoryPool is a linkedlist of MemoryHeader which is derieved from SLIST_ENTRY


// [MemoryHeader][Data]
// MemoryHeader => [SLIST_ENTRY][allocSize]

// same as __declspec(align(MEMORY_HEADER_ALIGN))
__declspec(align(MEMORY_ALLOCATION_ALIGNMENT))
struct MemoryHeader : public SLIST_ENTRY
{
public:
	MemoryHeader(int32 size);

	/// <summary>
	/// Attaches a new header.
	/// </summary>
	/// <param name="header">The pointer of the new header.</param>
	/// <param name="size">The size of the data.</param>
	/// <returns>The starting memory position of the data.</returns>
	static void* AttachHeader(MemoryHeader* header, const int32 size);
	/// <summary>
	/// Detaches the header.
	/// </summary>
	/// <param name="ptr">The pointer of the memory header to detach.</param>
	/// <returns>The pointer of previous memory header.</returns>
	static NetCore::MemoryHeader* DetachHeader(void* ptr);
private:
	int32 _allocSize;

public:
	friend class MemoryPool;
	friend class Memory;
};

NAMESPACE_CLOSE

