#ifndef MEM_TYPES_H
#define MEM_TYPES_H

#include <com/types.h>

struct mem_Chunk
{
	void *data;
	u64 size;
};

#endif