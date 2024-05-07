#pragma once
#include "MemoryPool.h"
#include "MemoryAllocator.h"

// READ
// https://learn.microsoft.com/en-us/windows/win32/Sync/using-singly-linked-lists




NAMESPACE_OPEN(NetCore)

class MemoryPool;

/// <summary>
/// Alloc a new object of Type on memory with specific allocator. (new operator)
/// </summary>
/// <typeparam name="Type">Type of object.</typeparam>
/// <typeparam name="...Args">The types of the arguments of constructor.</typeparam>
/// <param name="...args">Arguments of constructor of Type.</param>
/// <returns>The pointer of the allocated object.</returns>
template<typename Type, typename... Args>
[[nodiscard]] Type* xxnew(Args&&... args)
{
	Type* ptr = static_cast<Type*>(PoolAllocator::Alloc(sizeof(Type)));
	new(ptr)Type(std::forward<Args>(args)...);
	return ptr;
}

/// <summary>
/// Release allocated object which is allocated with NetCore::xxnew. (delete operator)
/// </summary>
/// <typeparam name="Type">Type of object.</typeparam>
/// <param name="ptr">The pointer of the object to release.</param>
template<typename Type>
void xxdelete(Type* ptr)
{
	ptr->~Type(); // Call destructor explicitly
	PoolAllocator::Release(ptr);
}

/// <summary>
/// Creates a std::shared_ptr with NetCore::xxnew and NetCore::xxdelete.
/// </summary>
/// <typeparam name="Type">The type of the object to be managed by the shared_ptr.</typeparam>
/// <typeparam name="...Args">Types of the arguments used to construct the object.</typeparam>
/// <param name="...args">Arguments forwarded to the constructor of the managed object.</param>
/// <returns>A std::shared_ptr with NetCore::xxnew and NetCore::xxdelete.</returns>
template<typename Type, typename... Args>
std::shared_ptr<Type> make_shared(Args&&... args)
{
	return std::shared_ptr<Type>
	{
		xxnew<Type>(std::forward<Args>(args)...), xxdelete<Type>
	};
}


/// <summary>
/// Creates a std::unique_ptr with NetCore::xxnew and NetCore::xxdelete.
/// </summary>
/// <typeparam name="Type">The type of the object to be managed by the unique_ptr.</typeparam>
/// <typeparam name="...Args">Types of the arguments used to construct the object.</typeparam>
/// <param name="...args">Arguments forwarded to the constructor of the managed object.</param>
/// <returns>A std::unique_ptr with NetCore::xxnew and NetCore::xxdelete.</returns>
template<typename Type, typename... Args>
std::unique_ptr<Type, void(*)(Type*)> make_unique(Args&&... args)
{
	// 사용자 정의 삭제자를 지정하여 std::unique_ptr를 생성
	return std::unique_ptr<Type, void(*)(Type*)>
	{
		xxnew<Type>(std::forward<Args>(args)...), xxdelete<Type>
	};
}


/*****************************
* Memory
******************************/

// Note
// /kr 고정된 크기를 가진 메모리 공간으로 분할 및 할당


class Memory
{
	enum
	{
		// Note: Since the size of the header is 16 bytes, the minimum value is 32. (for 16-bytes alignment due to SLIST_ENTRY)
		// The size of data is at least 16 bytes. (It may not be actual data size, but the compiler aligns it with 32-byte alignment)
		// So, the memory block should be located with unit of 32 bytes at minimum. (not 16 bytes)
		// Adiitionally, the minimum space of the data is 16 bytes. (32 - 16)
		// If the size of the block is 16, it can not have data.
		// 

		BLOCK_SECTION_512 = 0x200,
		BLOCK_SECTION_1024 = 0x400,
		BLOCK_SECTION_2048 = 0x800,
		BLOCK_SECTION_4096 = 0x1000,

		BLOCK_SIZE_32 = 0x20,
		BLOCK_SIZE_64 = 0x40, 
		BLOCK_SIZE_128 = 0x80,
		BLOCK_SIZE_256 = 0x100,
#ifdef USE_BIG_MEMORY_BLOCK
		BLOCK_SIZE_512 = 0x200,
		MAX_ALLOC_SIZE = 0x2000,
#else
		MAX_ALLOC_SIZE = 0x1000,
#endif
	};
public:
	Memory(const Memory&) = delete;
	Memory(Memory&&) = delete;
	Memory& operator=(const Memory&) = delete;
	Memory& operator=(Memory&&) = delete;

	Memory();
	~Memory();

	void* Allocate(const int32 size) const;
	void Release(void* ptr) const;

private:
	MemoryHeader* alloc_big(const int32& size) const;
	void free_big(void* ptr) const;
	void init_memory_pool(int32& tableKey, int32& pos, const int32 end, const int32 block_size);
	
private:
	std::vector<MemoryPool*> _pools;

	/// <summary>
	/// A table to get memory pool about the size. (Indexing)
	/// <!--Note: The memory size can be considered as a key of the pool table.-->
	/// </summary>
	MemoryPool* _poolTable[MAX_ALLOC_SIZE + 1];
};

NAMESPACE_CLOSE

