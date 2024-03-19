#include "pch.h"
#include "MemoryAllocator.h"
#include "Memory.h"

void* NetCore::Allocator::Alloc(int32 size)
{
	return ::malloc(size);
}

void NetCore::Allocator::Release(void* ptr)
{
	::free(ptr);
}

void* NetCore::PoolAllocator::Alloc(int32 size)
{
	return NetCore::GMemory->Allocate(size);
}

void NetCore::PoolAllocator::Release(void* ptr)
{
	GMemory->Release(ptr);
}
