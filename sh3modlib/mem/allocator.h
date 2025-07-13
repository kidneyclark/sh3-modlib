#ifndef MEM_ALLOCATOR_H
#define MEM_ALLOCATOR_H

#include <com/types.h>
#include <mem/front.h>
#include <cstddef>

// std compatible allocator
template <typename T, u64 m_Tag> class mem_Allocator
{
      public:
	typedef T value_type;
	typedef value_type *pointer;
	typedef const value_type *const_pointer;
	typedef value_type &reference;
	typedef const value_type &const_reference;
	typedef std::size_t size_type;
	typedef std::ptrdiff_t difference_type;

	template <class U> struct rebind
	{
		typedef mem_Allocator<U, m_Tag> other;
	};

	mem_Allocator(mem_Allocator<T, m_Tag> const &) noexcept
	{
	}
	template <class U>
	mem_Allocator(mem_Allocator<U, m_Tag> const &) noexcept
	{
	}
	~mem_Allocator() = default;

	mem_Allocator() noexcept
	{
		if (mem_GetContext() == nullptr)
			mem_CreateContext();
	}

	value_type *allocate(std::size_t n)
	{
		//printf("mem_Allocate(%llu)\n", n * sizeof(value_type));
		return reinterpret_cast<value_type *>(
		    mem_AllocChunk(n * sizeof(value_type), m_Tag).data);
	}

	void deallocate(mem_Allocator<T, m_Tag>::value_type *p,
	                std::size_t n) noexcept
	{
		//printf("mem_Deallocate\n");
		mem_FreeChunk((mem_Chunk){p, n}, m_Tag);
	}
	bool operator==(mem_Allocator<T, m_Tag> const &a)
	{
		return this == &a;
	}
	bool operator!=(mem_Allocator<T, m_Tag> const &a)
	{
		return !operator==(a);
	}
};

#endif