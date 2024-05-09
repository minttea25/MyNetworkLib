#pragma once

NAMESPACE_OPEN(NetCore);

class FBAllocator : public flatbuffers::Allocator
{
	// Note: uint8_t = _ubyte = unigned char

	// Inherited via Allocator
	uint8_t* allocate(size_t size) override
	{
		uint8_t* ptr = static_cast<uint8_t*>(NetCore::PoolAllocator::Alloc(static_cast<int32>(size)));
		new(ptr)uint8_t();
		return ptr;
	}

	void deallocate(uint8_t* p, size_t size) override
	{
		NetCore::PoolAllocator::Release(p);
	}
};

NAMESPACE_CLOSE;

