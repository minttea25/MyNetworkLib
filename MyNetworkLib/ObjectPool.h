#pragma once

NAMESPACE_OPEN(NetCore)

class MemoryPool;

template<typename Type>
class ObjectPool
{
	using PTYPE = Type*;
public:
	/// <summary>
	/// If there is extra object created before, returns it, otherwise returns created new one.
	/// </summary>
	template<typename... Args>
	static Type* Acquire(Args&&... args)
	{
		PTYPE memory = static_cast<PTYPE>(MemoryHeader::AttachHeader(s_pool.Acquire(), s_allocSize));
		
		// placement new
		new(memory)Type(std::forward<Args>(args)...);
		return memory;
	}

	/// <summary>
	/// Release the pooled object (contains calling destructor)
	/// </summary>
	static void Release(PTYPE obj)
	{
		// Call destructor explicitly
		obj->~Type();

		s_pool.Release(MemoryHeader::DetachHeader(obj));
	}

	/// <summary>
	/// If there is extra std::shared_ptr based on object created before, return it, otherwise returns created new std::shared_ptr.
	/// </summary>
	template<typename... Args>
	static std::shared_ptr<Type> make_shared(Args&&... args)
	{
		return std::shared_ptr<Type> { Acquire(std::forward<Args>(args)...), Release };
	}

	uint32 poolCount() const { return s_pool.poolCount(); }
	uint32 useCount() const { return s_pool.useCount(); }
private:
	static int32 s_allocSize;
	static MemoryPool s_pool;
};

template<typename Type>
int32 ObjectPool<Type>::s_allocSize = sizeof(Type) + sizeof(MemoryHeader);

template<typename Type>
MemoryPool ObjectPool<Type>::s_pool{ s_allocSize };


NAMESPACE_CLOSE