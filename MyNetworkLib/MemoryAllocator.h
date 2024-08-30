#pragma once

NAMESPACE_OPEN(NetCore)

class Allocator
{
public:
	/// <summary>
	/// allocation
	/// </summary>
	static void* Alloc(int32 size);

	/// <summary>
	/// free memory
	/// </summary>
	static void Release(void* ptr);
};

class PoolAllocator
{
public:
	/// <summary>
	/// allocation
	/// </summary>
	static void* Alloc(int32 size);
	/// <summary>
	/// free memory
	/// </summary>
	static void Release(void* ptr);
};

/// <summary>
/// Note: See xmemory -> allocator
/// </summary>
/// <typeparam name="_Ty">type name</typeparam>
template<typename _Ty>
class STLAllocator
{
public:
	using value_type = _Ty;
	using pointer = _Ty*;
	using const_pointer = const _Ty*;
	using reference = _Ty&;
	using const_reference = const _Ty&;
	using size_type = std::size_t;
	using difference_type = std::ptrdiff_t;
public:
	constexpr STLAllocator() noexcept = default;

	constexpr STLAllocator(const STLAllocator&) noexcept = default;
	template<typename _Other>
	constexpr STLAllocator(const STLAllocator<_Other>&) noexcept {}
	~STLAllocator() = default;
public:
	STLAllocator& operator=(const STLAllocator&) = default;
public:
	

	_Ty* allocate(const size_t count)
	{
		const int32 size = static_cast<int32>(count * sizeof(_Ty));
		return static_cast<_Ty*>(PoolAllocator::Alloc(size));
	}
	void deallocate(_Ty* const ptr, const size_t count)
	{
		PoolAllocator::Release(ptr);
	}

	template<typename _Other>
	bool operator==(const STLAllocator<_Other>&) { return true; }

	template<typename _Other>
	bool operator!=(const STLAllocator<_Other>&) { return false; }
};

NAMESPACE_CLOSE

