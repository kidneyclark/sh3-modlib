#ifndef CRK_TABULA_H
#define CRK_TABULA_H

#include <mem/front.h>
#include <crk/vector.h>

namespace crk
{

class Tabula
{
public:
	Tabula();

	const char *operator[](size_t index) const;
	size_t FindString(const char *str);

	size_t AddStringSafe(const char *str, size_t str_length);
	size_t AddString(const char *str);

	size_t Size() const;
	void Clear();

private:
	static constexpr size_t PADDING_BITS = 8;
	mem_Chunk chunk {nullptr, 0};
	crk::vector<size_t> string_table;
	size_t size {0};
};
} // namespace crk

#endif