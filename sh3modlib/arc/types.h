#ifndef ARC_TYPES_H
#define ARC_TYPES_H

#include <com/types.h>

struct arc_Cluster
{
	u32 id; // unique per cluster
	u32 vfile_count;
	const char *name;
};

struct arc_Vfile
{
	u32 id; // unique per vfile
	u16 index; // index inside cluster
	u32 cluster_id; // cluster that's containing this vfile
	const char *name;
};

#endif