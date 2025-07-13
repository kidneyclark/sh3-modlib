#ifndef ARC_TYPES_H
#define ARC_TYPES_H

#include <com/types.h>
#include <crk/vector.h>
#include <crk/tabula.h>

struct arc_table_cluster
{
	u32 vfile_count;
	i32 string_index;
};

struct arc_table_vfile
{
	u16 index;
	u16 cluster_index;
	i32 string_index;
};

struct arc_Table
{
	crk::vector<arc_table_cluster> clusters;
	crk::vector<arc_table_vfile> vfiles;
	crk::Tabula strings;
};

#endif