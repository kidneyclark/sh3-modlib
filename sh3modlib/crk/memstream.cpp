#include "memstream.h"

#include <mem/front.h>
#include <cstring>

constexpr u64 MemoryStream_tag = COM_4CHAR_TAG('m', 's', 'r', 'm');

using namespace crk;

MemoryStream::MemoryStream()
	: chunk(nullptr, 0)
{
	mem_CreateContext();
}

MemoryStream::~MemoryStream()
{
	if (chunk.data != nullptr)
		mem_FreeChunk(chunk, MemoryStream_tag);
}

void MemoryStream::SetCapacity(size_t new_capacity)
{
	chunk = mem_ReallocChunk(chunk, new_capacity, MemoryStream_tag);

	if (size > chunk.size)
		size = chunk.size; // size shrunken
}

void MemoryStream::SetSize(size_t new_size)
{
	size = new_size;
	if (size > chunk.size)
		SetCapacity(size); // expand capacity
}

void MemoryStream::AddCapacity(size_t new_capacity)
{
	SetCapacity(chunk.size + new_capacity);
}

void MemoryStream::AddSize(size_t new_size)
{
	SetSize(size + new_size);
}

void *MemoryStream::AtPos()
{
	return (void *)(((uint8_t *)chunk.data) + pos);
}

void MemoryStream::MovePos(size_t off, bool forward)
{
	if (forward)
		pos += off;
	else
		pos -= off;
}

void MemoryStream::SetPos(size_t off, bool from_start)
{
	// assert((off < capacity) && "MemBuf::SetPos: Position
	// indicator went over capacity.");
	if (from_start)
		pos = off;
	else
		pos = chunk.size - off;
}

bool MemoryStream::IsPosValid()
{
	return pos < chunk.size;
}