#ifndef ARC_IMPL_TYPES_H
#define ARC_IMPL_TYPES_H

#include <com/types.h>
#include <crk/vector.h>
#include <crk/tabula.h>

struct _impl_arc_TableCluster
{
	u32 vfile_count;
	i32 string_index;
};

struct _impl_arc_TableVfile
{
	u16 index;
	u16 cluster_index;
	i32 string_index;
};

struct _impl_arc_TableItem
{
	u16 type;
	u16 size;
};

struct _impl_arc_Table
{
	crk::vector<_impl_arc_TableCluster> clusters;
	crk::vector<_impl_arc_TableVfile> vfiles;
	crk::Tabula strings;
};

#endif