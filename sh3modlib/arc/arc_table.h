#ifndef SH3ML_ARC_TABLE_H
#define SH3ML_ARC_TABLE_H

#include "arc.h"
#include <stdint.h>

struct arc_table_cluster
{
	uint32_t vfile_count;
	int string_index;
};

struct arc_table_vfile
{
	uint16_t index;
	uint16_t cluster_index;
	int string_index;
};

struct arc_table
{
	struct arc_table_cluster *clusters;
	size_t cluster_count;
	struct arc_table_vfile *vfiles;
	size_t vfile_count;
	char *strings;
	size_t strings_length;
};

int arc_table_load(struct arc_ctx *ctx, struct arc_table *table);
int arc_table_get_cluster_index(struct arc_table *table, const char *cluster);

#endif