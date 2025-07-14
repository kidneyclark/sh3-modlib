#ifndef MEM_TYPES_H
#define MEM_TYPES_H

#include <com/types.h>

struct mem_Chunk
{
	void *data;
	u64 size;
};

// Facilitates the use of mem_Chunk.
// Stores '.count' elements of T.
template <typename T>
struct mem_Array
{
	T *array;
	u64 count;

	T &operator[](size_t idx) { return array[idx]; }
};

#endif