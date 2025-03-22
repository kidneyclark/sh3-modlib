#ifndef ARC_TABLE_H
#define ARC_TABLE_H

struct arc_table_cluster
{
	int vfile_count;
	int string_index;
};

struct arc_table
{
	struct arc_table_cluster *clusters;
	int cluster_count;
};

extern void arc_test(void);

#endif