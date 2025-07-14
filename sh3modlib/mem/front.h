#ifndef MEM_FRONT_H
#define MEM_FRONT_H

#include "types.h"
#include <cstdio>

#include <com/defs.h>
COM_CONTEXT_DECL(mem)

mem_Chunk mem_AllocChunk(u64 size, u64 tag);
void mem_FreeChunk(mem_Chunk chunk, u64 tag);
mem_Chunk mem_ReallocChunk(mem_Chunk chunk, u64 size, u64 tag);

template <typename T>
mem_Array<T> mem_AllocArray(u64 count, u64 tag)
{
	mem_Chunk chunk = mem_AllocChunk(count * sizeof(T), tag);
	return mem_Array<T>{.array = (T*)chunk.data, .count = count};
}

template <typename T>
void mem_FreeArray(mem_Array<T> array, u64 tag)
{
	mem_Chunk chunk = {.data = array.array, .size = array.count * sizeof(T)};
	mem_FreeChunk(chunk, tag);
}

template <typename T>
mem_Array<T> mem_ReallocArray(mem_Array<T> array, u64 count, u64 tag)
{
	mem_Chunk chunk = {.data = array.array, .size = array.count * sizeof(T)};
	chunk = mem_ReallocChunk(chunk, count * sizeof(T), tag);
	return mem_Array<T>{.array = (T*)chunk.data, .count = count};
}

#include <ostream>
void mem_Dump(std::ostream &out);

void mem_RegisterTag(u64 tag, const char *name);

#endif