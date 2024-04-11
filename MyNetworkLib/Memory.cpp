#include "pch.h"
#include "Memory.h"



NetCore::Memory::Memory()
{
	/*
	* [Pool32][Pool64][Pool128]	...	[Pool512][Pool578]	... [Pool1024][[Pool1152] ...
	*						512						1024						2048
	* Note: PoolXX means MemoryPool(XX), the size of the header is XX bytes.
	* In XX bytes, the first 16 bytes are header and the remaining bytes are data.
	*/

	int32 pos = BLOCK_SIZE_32; // miminum size
	int32 tableNumber = 0;

	// Divide memory to fixed-size block

	init_memory_pool(tableNumber, pos, BLOCK_SECTION_1024, BLOCK_SIZE_32);
	init_memory_pool(tableNumber, pos, BLOCK_SECTION_2048, BLOCK_SIZE_128);

#ifdef USE_BIG_MEMORY_BLOCK
	init_memory_pool(tableNumber, pos, BLOCK_SECTION_4096, BLOCK_SIZE_256);
	init_memory_pool(tableNumber, pos, MAX_ALLOC_SIZE, BLOCK_SIZE_512);
#else
	init_memory_pool(tableNumber, pos, MAX_ALLOC_SIZE, BLOCK_SIZE_256);
#endif // USE_BIG_MEMORY_BLOCK
	auto t = _poolTable[32];
}

NetCore::Memory::~Memory()
{
	for (MemoryPool* pool : _pools) delete pool;

	_pools.clear();
}

void* NetCore::Memory::Allocate(const int32 size) const
{
	// Create new header ------------------------------------------------------
	MemoryHeader* header = nullptr;
	const int32 allocSize = size + sizeof(MemoryHeader);

	// Check the size
	if (allocSize > MAX_ALLOC_SIZE)
	{
		// If the size is bigger than MAX_ALLOC_SIZE, alloc new instead using pool.
		alloc_big(allocSize);
	}
	else
	{
		// Lends memory from the pool.
		// allocSize is byte quantity and it is used as index.
		// It may cause C6305 warning.
		header = _poolTable[allocSize]->Acquire();
	}
	// -------------------------------------------------------------------------

#ifdef TEST
	//SHOW(pooltable, _poolTable[allocSize]);
	//std::cout << _poolTable[allocSize] << std::endl;
#endif // TEST

	return MemoryHeader::AttachHeader(header, allocSize);
}

void NetCore::Memory::Release(void* ptr) const
{
	// Get memoryheader
	MemoryHeader* header = MemoryHeader::DetachHeader(ptr);

	const int32 allocSize = header->_allocSize;

	// AllocSize is not 0

	if (allocSize > MAX_ALLOC_SIZE)
	{
		free_big(header);
	}
	else
	{
		// Release to the pool.
		_poolTable[allocSize]->Release(header);

#ifdef TEST
		//SHOW(pooltable, _poolTable[allocSize]);
		//std::cout << _poolTable[allocSize] << std::endl;
#endif // TEST

	}
}

NetCore::MemoryHeader* NetCore::Memory::alloc_big(const int32& size) const
{
	return reinterpret_cast<MemoryHeader*>(::_aligned_malloc(size, MEMORY_ALLOCATION_ALIGNMENT));
}

void NetCore::Memory::free_big(void* ptr) const
{
	::_aligned_free(ptr);
}

void NetCore::Memory::init_memory_pool(int32& tableKey, int32& pos, const int32 end, const int32 block_size)
{
	for (; pos <= end; pos += block_size)
	{
		MemoryPool* pool = new MemoryPool(pos);
		_pools.push_back(pool);

		while (tableKey <= pos)
		{
			_poolTable[tableKey] = pool;
			tableKey++;
		}
	}
}
