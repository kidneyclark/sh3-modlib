#ifndef CRK_VECTOR_H
#define CRK_VECTOR_H

#include <vector>
#include <mem/allocator.h>
#include <com/defs.h>

namespace crk
{
const u64 vector_tag = COM_4CHAR_TAG('_', 'v', 't', 'r');
template <typename T, u64 _tag = vector_tag>
using vector =
    std::vector<T, mem_Allocator<T, _tag>>;
}

#endif