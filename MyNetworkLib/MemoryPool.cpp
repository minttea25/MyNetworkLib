#include "pch.h"
#include "MemoryPool.h"

NetCore::MemoryPool::MemoryPool(int32 size) : _allocSize(size)
{
    ::InitializeSListHead(&_header);
}

NetCore::MemoryPool::~MemoryPool()
{
    // Note: It returns removed entry pointer. (If list is empty, returns NULL)
    // Removes all headers
    while (MemoryHeader* memory = static_cast<MemoryHeader*>(::InterlockedPopEntrySList(&_header)))
    {
        // Free aligned memory
        ::_aligned_free(memory); // winapi
    }
}

void NetCore::MemoryPool::Release(MemoryHeader* ptr)
{
    // MemoryHeader::_allocSize=0 means `It is not in use now`.
    ptr->_allocSize = 0;

    // Note: It returns previous entry pointer. (If list was empty, returns NULL)
    // Returns the memory to the pool
    ::InterlockedPushEntrySList(&_header, static_cast<PSLIST_ENTRY>(ptr));

    // Increase the pool count
    _poolCount.fetch_add(1);

    // Decrease the use count
    _useCount.fetch_sub(1);
}

[[nodiscard]]
NetCore::MemoryHeader* NetCore::MemoryPool::Acquire()
{
    // Note: If list is empty, ::InterlockedPopEntrySList returns nullptr.
    PSLIST_ENTRY entry = ::InterlockedPopEntrySList(&_header);
    // So, nullptr can not be casted to MemoryHeader* and header will be nullptr.
    MemoryHeader* header = static_cast<MemoryHeader*>(entry);

    // If list is empty, make new one
    if (header == nullptr)
    {
        // Alloc new header with aligned bytes.
        header = reinterpret_cast<MemoryHeader*>(::_aligned_malloc(_allocSize, MEMORY_ALLOCATION_ALIGNMENT));
        ASSERT_CRASH(header != nullptr);

        // CHECK
        header->_allocSize = _allocSize;
    }
    else
    {
        _poolCount.fetch_sub(1);
    }

    _useCount.fetch_add(1);

    return header;
}

NetCore::MemoryHeader::MemoryHeader(int32 size) : _allocSize(size)
{
}

void* NetCore::MemoryHeader::AttachHeader(MemoryHeader* header, const int32 size)
{
    new(header)MemoryHeader(size); // placement new
    // Jumps the size of memory header (with ++)
    // Returns the pointer of starting position of next memory header
    return reinterpret_cast<void*>(++header);
}

NetCore::MemoryHeader* NetCore::MemoryHeader::DetachHeader(void* ptr)
{
    // Get the pointer of the previous memory header.
    auto t = reinterpret_cast<MemoryHeader*>(ptr);
    MemoryHeader* header = reinterpret_cast<MemoryHeader*>(ptr) - 1;
    return header;
}
