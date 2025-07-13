#ifndef CRK_STRING_H
#define CRK_STRING_H

#include <com/defs.h>
#include <mem/allocator.h>
#include <string>

namespace crk
{
const u64 string_tag = COM_4CHAR_TAG('_', 's', 't', 'r');
template <u64 _tag = string_tag>
using string =
    std::basic_string<char, std::char_traits<char>, mem_Allocator<char, _tag>>;
} // namespace crk

#endif