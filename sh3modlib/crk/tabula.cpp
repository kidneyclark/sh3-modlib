#include "tabula.h"

#include <stdlib.h>
#include <string.h>
#include <assert.h>

using namespace crk;

constexpr u64 Tabula_Tag = COM_4CHAR_TAG('t', 'a', 'b', '1');

Tabula::Tabula()
{
	mem_CreateContext();
}

const char *Tabula::operator[](size_t index) const
{
	assert(index < string_table.size());
	return (char*)chunk.data + string_table[index];
}

size_t Tabula::FindString(const char *str)
{
	int i = 0;
	for (size_t it : string_table)
	{
		if (strcmp((char*)chunk.data + string_table[it], str) == 0)
		{
			break;
		}
		i++;
	}
	return i;
}

size_t Tabula::AddStringSafe(const char *str, size_t str_length)
{
	size_t start_pos = size;
	size_t new_size = size + str_length + 1;
	if (new_size > chunk.size)
	{
		size_t new_cap = (new_size + (1<<PADDING_BITS)) & ~((1<<PADDING_BITS)-1);
		chunk = mem_ReallocChunk(chunk, new_cap, Tabula_Tag);
	}
	size = new_size;
	strcpy(((char*)chunk.data + start_pos), str);
	((char*)chunk.data)[start_pos + str_length] = '\0';

	string_table.push_back(start_pos);

	return start_pos;
}

size_t Tabula::AddString(const char *str)
{
	return AddStringSafe(str, strlen(str));
}

size_t Tabula::Size() const
{
	return string_table.size();
}

void Tabula::Clear()
{
	string_table.clear();
	mem_FreeChunk(chunk, Tabula_Tag);
	chunk.data = nullptr;
	chunk.size = 0;
	size = 0;
}