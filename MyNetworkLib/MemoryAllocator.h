#pragma once

NAMESPACE_OPEN(NetCore)

class Allocator
{
public:
	static void* Alloc(int32 size);
	static void Release(void* ptr);
};

class PoolAllocator
{
public:
	static void* Alloc(int32 size);
	static void Release(void* ptr);
};

NAMESPACE_CLOSE

