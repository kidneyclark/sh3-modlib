#ifndef MEM_FRONT_H
#define MEM_FRONT_H

#include "types.h"
#include <cstdio>

#include <com/defs.h>
COM_CONTEXT_DECL(mem)

mem_Chunk mem_AllocChunk(u64 size, u64 tag);
void mem_FreeChunk(mem_Chunk chunk, u64 tag);
mem_Chunk mem_ReallocChunk(mem_Chunk chunk, u64 size, u64 tag);

#include <ostream>
void mem_Dump(std::ostream &out);

void mem_RegisterTag(u64 tag, const char *name);

#endif