#ifndef ARC_FRONT_H
#define ARC_FRONT_H

#include "types.h"

#include <com/defs.h>
COM_CONTEXT_DECL(arc)

#include <istream>
arc_Table arc_CreateTableFromStream(std::istream &in);
size_t arc_GetTableClusterIndex(arc_Table &table, const char *cluster);

#include <mem/types.h>

#endif